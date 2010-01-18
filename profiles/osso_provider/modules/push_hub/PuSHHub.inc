<?php

/**
 * @file
 * PubsubHubbub library.
 */

/**
 * A PubsubHubbub Hub.
 */
class PuSHHub {
  // Class to use for storing subscriptions.
  protected $subscriptions;

  /**
   * Singleton.
   */
  public function instance(PuSHHubSubscriptionsInterface $subscriptions) {
    static $hub;
    if (empty($hub)) {
      $hub = new PuSHHub($subscriptions);
    }
    return $hub;
  }

  /**
   * Constructor.
   *
   * @param $subscriptions
   *   Subsriptions object that handles subscription storage. Must implement
   *   PuSHHubSubscriptionsInterface.
   */
  protected function __construct(PuSHHubSubscriptionsInterface $subscriptions) {
    $this->subscriptions = $subscriptions;
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
    foreach ($this->subscriptions->all($topic) as $subscriber) {
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
      $this->subscriptions->save($post['hub_topic'], $post['hub_callback']);
      header('HTTP/1.0 204 "No Content"', null, 204);
      exit();
    }
    header('HTTP/1.0 404 "Not Found"', null, 404);
    exit();
  }
}

/**
 * Describes the storage for PuSHHub subscriptions. Implement to provide a
 * storage class.
 */
interface PuSHHubSubscriptionsInterface {

  /**
   * Save a subscription.
   */
  public function save($topic, $subscriber);

  /**
   * Delete a subscription.
   */
  public function delete($topic, $subscriber);

  /**
   * Find all subscriber URLs for a given topic URL.
   *
   * @return
   *   An array of subscriber URLs.
   */
  public function all($topic);
}
