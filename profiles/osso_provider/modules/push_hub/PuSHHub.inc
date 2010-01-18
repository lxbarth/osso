<?php

/**
 * @file
 * A PubsubHubbub Hub.
 */

class PuSHHub {
  // Class to use for creating a subscriber object.
  protected $subscriber_class;

  /**
   * Singleton.
   */
  public function instance($subscriber_class) {
    static $hub;
    if (empty($hub)) {
      $hub = new PuSHHub($subscriber_class);
    }
    return $hub;
  }

  /**
   * Constructor.
   *
   * @param $subscriber_class
   *   String that identifies a subscriber class. Must implement
   *   PuSHHubSubscriptionInterface.
   */
  protected function __construct($subscriber_class) {
    $this->subsciber_class = $subscriber_class;
  }

  /**
   * Notify subscribers of a change in a topic.
   *
   * @param $topic
   *   URL of the topic that changed.
   * @param $changed
   *   The feed that contains the changed elements.
   */
  public function notify($topic, $changed) {
    foreach ($this->subscribers($topic) as $subscriber) {
      $this->notifySubscriber($subscriber, $topic, $changed);
    }
  }

  /**
   * Notify a specific subscriber of a change in a topic.
   *
   * @todo: Fix POST content type.
   * @todo: Support a light ping.
   */
  public function notifySubscriber($subscriber, $topic, $changed) {
    $request = curl_init($subscriber);
//    curl_setopt($request, CURLOPT_HTTPHEADER, array("Content-Type: application/atom+xml; charset=utf-8"));
    curl_setopt($request, CURLOPT_POST, TRUE);
    curl_setopt($request, CURLOPT_POSTFIELDS, array('changed' => $changed));
    curl_exec($request);
    $code = curl_getinfo($request, CURLINFO_HTTP_CODE);
    if ($code < 200 || $code > 299) {
      error_log('Error notifying '. $subscriber .' of change in '. $topic);
    }
    curl_close($request);
  }

  /**
   * Verify subscription request.
   */
  public function verify($post) {
    // Send verification request to subscriber.
    $challenge = md5(session_id() . rand());
    $query = array(
      'hub.mode='. $post['hub_mode'],
      'hub.topic='. urlencode($post['hub_topic']),
      'hub.challenge='. $challenge,
      'hub.lease_seconds='. $post['hub_lease_seconds'],
      'hub.verify_token='. $post['hub_verify_token'],
    );
    $parsed = parse_url($post['hub_callback']);
    $request = curl_init($post['hub_callback'] . (empty($parsed['query']) ? '?' : '&') . implode('&', $query));
    curl_setopt($request, CURLOPT_FOLLOWLOCATION, TRUE);
    curl_setopt($request, CURLOPT_RETURNTRANSFER, TRUE);
    $data = curl_exec($request);
    $code = curl_getinfo($request, CURLINFO_HTTP_CODE);
    curl_close($request);
    if ($code > 199 && $code < 300 && $data == $challenge) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Handle a subscription request.
   *
   * @param $post
   *   A valid PubSubHubbub subscription request.
   */
  public function subscribe($post) {
    if (isset($post['hub_topic']) && isset($post['hub_callback']) && $this->verify($post)) {
      $this->saveSubscription(array('topic' => $post['hub_topic'], 'subscriber' => $post['hub_callback'], 'timestamp' => time()));
      header('HTTP/1.0 204 "No Content"', null, 204);
      exit();
    }
    header('HTTP/1.0 404 "Not Found"', null, 404);
    exit();
  }

  /**
   * Save a notification subscription.
   *
   * @param $subscription
   *   A subscription array with at least a 'topic' and a 'subscriber' key.
   */
  protected function saveSubscription($subscription) {
    $this->deleteSubscription($subscription);
    drupal_write_record('push_hub_subscriptions', $subscription);
  }

  /**
   * Delete subscription.
   *
   * @param $subscription
   *   A subscription array with a 'topic' and a 'subscriber' key.
   */
  protected function deleteSubscription($subscription) {
    db_query("DELETE FROM {push_hub_subscriptions} WHERE topic = '%s' AND subscriber = '%s'", $subscription['topic'], $subscription['subscriber']);
  }

  /**
   * Return all subscribers for a given topic URL as an array of subscriber URLs.
   */
  protected function subscribers($topic) {
    $subscribers = array();
    $result = db_query("SELECT subscriber FROM {push_hub_subscriptions} WHERE topic = '%s'", $topic);
    while ($row = db_fetch_object($result)) {
      $subscribers[] = $row->subscriber;
    }
    return $subscribers;
  }
}

/**
 * Describes a PuSHHubSubscription. Implement to provide persistent storage.
 */
interface PuSHHubSubscriptionInterface {
  /**
   * Constructor.
   */
  public function __construct($topic, $subscriber, $timestamp);

  /**
   * Save a subscription.
   */
  public function save();

  /**
   * Delete a subscription.
   */
  public function delete();

  /**
   * Find all subscriber URLs for a given topic URL.
   *
   * @return
   *   An array of subscriber URLs.
   */
  public static function all($topic);
}
