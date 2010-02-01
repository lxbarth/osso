<?php

/**
 * @file
 * Pubsubhubbub subscriber library.
 */

/**
 * Pubsub subscriber.
 */
class PuSHSubscriber {
  protected $domain;
  protected $subscriber_id;
  protected $subscriptions;

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
   * @param PuSHSubscriptionsInterface $subscriptions
   *   An object to use for storing and loading subscriptions.
   */
  public static function instance($domain, $subscriber_id, PuSHSubscriptionsInterface $subscriptions) {
    static $subscribers;
    if (!isset($subscriber[$domain][$subscriber_id])) {
      $subscriber = new PuSHSubscriber($domain, $subscriber_id, $subscriptions);
    }
    return $subscriber;
  }

  /**
   * Protect constructor.
   */
  protected function __construct($domain, $subscriber_id, PuSHSubscriptionsInterface $subscriptions) {
    $this->domain = $domain;
    $this->subscriber_id = $subscriber_id;
    $this->subscriptions = $subscriptions;
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
    $request = curl_init($url);
    curl_setopt($request, CURLOPT_FOLLOWLOCATION, TRUE);
    curl_setopt($request, CURLOPT_RETURNTRANSFER, TRUE);
    $data = curl_exec($request);
    if (curl_getinfo($request, CURLINFO_HTTP_CODE) == 200) {
      $xml = new SimpleXMLElement($data);
      $xml->registerXPathNamespace('atom', 'http://www.w3.org/2005/Atom');
      if ($hub = current($xml->xpath("/atom:feed/atom:link[attribute::rel='hub']"))) {
        $hub = (string) $hub->attributes()->href;
      }
      if ($self = current($xml->xpath("/atom:feed/atom:link[attribute::rel='self']"))) {
        $self = (string) $self->attributes()->href;
      }
      if ($hub && $self) {
        $this->request($hub, $self, 'subscribe', $callback_url);
      }
    }
    curl_close($request);
  }

  /**
   * @todo Unsubscribe from a hub.
   * @todo Make sure we unsubscribe with the correct topic URL as it can differ
   * from the initial subscription URL.
   *
   * @param $topic_url
   *   The URL of the topic to unsubscribe from.
   * @param $callback_url
   *   The callback to unsubscribe.
   */
  public function unsubscribe($topic_url, $callback_url) {
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
    $this->subscriptions->save($this->domain, $this->subscriber_id, $hub, $topic, $mode, $post_fields);
    // Issue subscription request.
    $request = curl_init($hub);
    curl_setopt($request, CURLOPT_POST, TRUE);
    curl_setopt($request, CURLOPT_POSTFIELDS, $post_fields);
    curl_exec($request);
    if (in_array(curl_getinfo($request, CURLINFO_HTTP_CODE), array(202, 204))) {
      $this->subscriptions->save($this->domain, $this->subscriber_id, $hub, $topic, '');
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
    if ($subscription = $this->subscriptions->load($this->domain, $this->subscriber_id)) {
      if ($verify_fields['hub_verify_token'] == $subscription['post_fields']['hub.verify_token']) {
        header('HTTP/1.1 200 "Found"', null, 200);
        print $verify_fields['hub_challenge'];
        exit();
      }
    }
    header('HTTP/1.1 404 "Not Found"', null, 404);
    exit();
  }

  /**
   * Receive a notification.
   */
  public function receive() {
    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
      return file_get_contents('php://input');
    }
    return FALSE;
  }
}

/**
 * Implement to provide a storage backend for subscriptions.
 */
interface PuSHSubscriptionsInterface {
  /**
   * Save a subscription.
   *
   * @param $domain
   *   A string that defines the domain in which the subscriber_id is unique.
   * @param $subscriber_id
   *   A unique numeric subscriber id.
   * @param $hub
   *   The URL of the hub endpoint.
   * @param $topic
   *   The topic to subscribe to.
   * @param $mode
   *   The temporary mode of the subscription.
   * @param $post_fields
   *   An array of the fields posted to the hub.
   */
  public function save($domain, $subscriber_id, $hub, $topic, $mode, $post_fields = array());

  /**
   * Load a subscription.
   */
  public function load($domain, $subscriber_id);

  /**
   * Delete a subscription.
   */
  public function delete($domain, $subscriber_id);
}
