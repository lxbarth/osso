<?php

/**
 * @file
 * Pubsubhubbub subscriber library.
 *
 * Requirements:
 *
 * PHP 5.2.x or higher
 * SimpleXML
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
   */
  public function subscribe($url) {
    if (valid_url($url)) {
      // Fetch document, find rel=hub and rel=self.
      // If present, issue subscription request.
      feeds_include_library('http_request.inc', 'http_request');
      if (!http_request_use_curl()) {
        throw new Exception(t('CURL required.'));
      }
      $result = http_request_get($url);
      if ($result->code == 200) {
        $xml = new SimpleXMLElement($result->data);
        $xml->registerXPathNamespace('atom', 'http://www.w3.org/2005/Atom');
        $hub = (string) current($xml->xpath("/atom:feed/atom:link[attribute::rel='hub']"))->attributes()->href;
        $self = (string) current($xml->xpath("/atom:feed/atom:link[attribute::rel='self']"))->attributes()->href;
        if ($hub && $self) {
          $this->request($hub, $self, 'subscribe');
        }
      }
    }
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
   *
   * Compare to http://pubsubhubbub.googlecode.com/svn/trunk/pubsubhubbub-core-0.2.html#anchor5
   */
  protected function request($hub, $topic, $mode) {
    $post_fields = array(
      'hub.callback' => url('feeds/pubsubhubbub/notify/'. $this->id .'/'. $this->subscriber_id, array('absolute' => TRUE)),
      'hub.mode' => $mode,
      'hub.topic' => $topic,
      'hub.verify' => 'sync',
      'hub.lease_seconds' => '', // Permanent subscription.
      'hub.secret' => '', // @todo
      'hub.verify_token' => drupal_get_token(),
    );
    $this->saveRequest($post_fields);
    // Issue subscription request.
    $request = curl_init($hub);
    curl_setopt($request, CURLOPT_POST, TRUE);
    curl_setopt($request, CURLOPT_POSTFIELDS, $post_fields);
    curl_exec($request);
    if (!in_array(curl_getinfo($request), array(202, 204))) {
      drupal_set_message(t('Error subscribing to PubsubHubbub hub.'), 'error');
    }
    curl_close($request);
  }

  /**
   * Verify subscription request.
   */
  public function verifySubscriptionRequest($verify_fields) {
    if ($sent_fields = $this->loadRequest()) {
      if ($sent_fields['hub.verify_token'] == $verify_fields['hub.verify_token']) {
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
   * Save a subscription request. This is Drupal-dependent. Override for other
   * storage backends.
   */
  protected function saveRequest($post_fields) {
    // Cache request for verification.
    $save = array(
      'domain' => $this->domain,
      'subscriber_id' => $this->subscriber_id,
      'topic' => $values['hub.topic'],
      'mode' => $values['hub.mode'],
      'post_fields' => $post_fields,
    );
    $this->deleteRequest();
    drupal_write_record('feeds_push_requests', $save);
  }

  /**
   * Load a subscription request. This is Drupal-dependent. Override for other
   * storage backends.
   */
  protected function loadRequest() {
    if ($request = db_fetch_array(db_query("SELECT * FROM {feeds_push_requests} WHERE domain = '%s' AND subscriber_id = %d", $this->domain, $this->subscriber_id))) {
      $request['post_fields'] = unserialize($request['post_fields']);
    }
  }

  /**
   * Delete a subscription request. This is Drupal-dependent. Override for other
   * storage backends.
   */
  protected function deleteRequest() {
    db_query("DELETE FROM {feeds_push_requests} WHERE domain = '%s' AND subscriber_id = %d", $this->domain, $this->subscriber_id);
  }
}