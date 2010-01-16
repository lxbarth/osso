<?php

/**
 * @file
 * Pubsubhubbub subscriber library.
 *
 * Requirements:
 *
 * PHP 5.2.x or higher
 * SimpleXML
 * cURL
 *
 * @todo Abstract storage layer.
 */

/**
 * Pubsub subscriber.
 */
class PuSHSubscriber {
  protected $domain;
  protected $subscriber_id;

  /**
   * Singleton.
   *
   * PuSHSubscriber identifies a unique subscription by a domain and a numeric
   * id. The numeric id is assumed to e unique in its domain.
   *
   * @param $domain
   *   A string that identifies the domain in which $subscriber_id is unique.
   * @param $subscriber_id
   *   A numeric subscriber id.
   */
  public static function instance($domain, $subscriber_id) {
    static $subscribers;
    if (!isset($subscriber[$domain][$subscriber_id])) {
      $subscriber = new PuSHSubscriber($domain, $subscriber_id);
    }
    return $subscriber;
  }

  /**
   * Protect constructor.
   */
  protected function __construct($domain, $subscriber_id) {
    $this->domain = $domain;
    $this->subscriber_id = $subscriber_id;
  }

  /**
   * Subscribe to a given URL. Retrieve 'hub' and 'self' links from feed at $url
   * and issue a subscription request to the hub.
   *
   * @param $url
   *   The URL of the feed to subscribe to.
   * @param $callback_url
   *   The full URL that hub should invoke when feed changes.
   */
  public function subscribe($url, $callback_url) {
    // Fetch document, find rel=hub and rel=self.
    // If present, issue subscription request.
    feeds_include_library('http_request.inc', 'http_request');
    $request = curl_init($url);
    curl_setopt($request, CURLOPT_FOLLOWLOCATION, TRUE);
    curl_setopt($request, CURLOPT_RETURNTRANSFER, TRUE);
    $data = curl_exec($request);
    if (curl_getinfo($request, CURLINFO_HTTP_CODE) == 200) {
      $xml = new SimpleXMLElement($data);
      $xml->registerXPathNamespace('atom', 'http://www.w3.org/2005/Atom');
      $hub = (string) current($xml->xpath("/atom:feed/atom:link[attribute::rel='hub']"))->attributes()->href;
      $self = (string) current($xml->xpath("/atom:feed/atom:link[attribute::rel='self']"))->attributes()->href;
      if ($hub && $self) {
        $this->request($hub, $self, 'subscribe', $callback_url);
      }
    }
    curl_close($request);
  }

  /**
   * Issue a subscribe or unsubcribe request to a PubsubHubbub hub.
   *
   * @param $hub
   *   The URL of the hub's subscription endpoint.
   * @param $topic
   *   The topic URL of the feed to subscribe to.
   * @param $mode
   *   'subscribe' or 'unsubscribe'.
   * @param $callback_url
   *   The subscriber's notifications callback URL.
   *
   * Compare to http://pubsubhubbub.googlecode.com/svn/trunk/pubsubhubbub-core-0.2.html#anchor5
   *
   * @todo don't use url()
   */
  protected function request($hub, $topic, $mode, $callback_url) {
    $post_fields = array(
      'hub.callback' => $callback_url,
      'hub.mode' => $mode,
      'hub.topic' => $topic,
      'hub.verify' => 'sync',
      'hub.lease_seconds' => '', // Permanent subscription.
      'hub.secret' => '', // @todo
      'hub.verify_token' => md5(session_id() . rand()),
    );
    $this->saveSubscription($hub, $topic, $mode, $post_fields);
    // Issue subscription request.
    $request = curl_init($hub);
    curl_setopt($request, CURLOPT_POST, TRUE);
    curl_setopt($request, CURLOPT_POSTFIELDS, $post_fields);
    curl_exec($request);
    if (in_array(curl_getinfo($request, CURLINFO_HTTP_CODE), array(202, 204))) {
      $this->saveSubscription($hub, $topic, '');
    }
    else {
      drupal_set_message(t('Error subscribing to PubsubHubbub hub.'), 'error');
    }
    curl_close($request);
  }

  /**
   * Verify subscription request.
   *
   * @todo: break out subscription handling in its own abstract class with a
   * Drupal implementer.
   */
  public function verifySubscriptionRequest($verify_fields) {
    if ($subscription = $this->loadSubscription()) {
      if ($verify_fields['hub_verify_token'] == $subscription['post_fields']['hub.verify_token']) {
        header('HTTP/1.0 200 "Found"', null, 200);
        print $verify_fields['hub_challenge'];
        exit();
      }
    }
    header('HTTP/1.0 404 "Not Found"', null, 404);
    exit();
  }

  /**
   * Receive a notification.
   */
  public function pushReceive($post) {
error_log("Received !". print_r($post, true));
  }

  /**
   * Save a subscription. This is Drupal-dependent. Override for other
   * storage backends.
   */
  protected function saveSubscription($hub, $topic, $mode, $post_fields = array()) {
    // Cache request for verification.
    $save = array(
      'domain' => $this->domain,
      'subscriber_id' => $this->subscriber_id,
      'hub' => $hub,
      'topic' => $topic,
      'mode' => $mode,
      'post_fields' => $post_fields,
      'timestamp' => time(),
    );
    $this->deleteSubscription();
    drupal_write_record('feeds_push_subscriptions', $save);
  }

  /**
   * Load a subscription. This is Drupal-dependent. Override for other
   * storage backends.
   */
  protected function loadSubscription() {
    if ($request = db_fetch_array(db_query("SELECT * FROM {feeds_push_subscriptions} WHERE domain = '%s' AND subscriber_id = %d", $this->domain, $this->subscriber_id))) {
      $request['post_fields'] = unserialize($request['post_fields']);
    }
    return $request;
  }

  /**
   * Delete a subscription. This is Drupal-dependent. Override for other
   * storage backends.
   */
  protected function deleteSubscription() {
    db_query("DELETE FROM {feeds_push_subscriptions} WHERE domain = '%s' AND subscriber_id = %d", $this->domain, $this->subscriber_id);
  }
}