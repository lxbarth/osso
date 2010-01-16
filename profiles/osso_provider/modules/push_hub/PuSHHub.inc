<?php

/**
 * @file
 * A PubsubHubbub Hub.
 */

class PuSHHub {

  /**
   * Singleton.
   */
  public function instance() {
    static $hub;
    if (empty($hub)) {
      $hub = new PuSHHub();
    }
    return $hub;
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
   */
  public function notifySubscriber($subscriber, $topic, $changed) {
error_log(print_r(func_get_args(), true));
    $request = curl_init($subscriber);
    curl_setopt($request, CURLOPT_HEADER, TRUE);
    curl_setopt($request, CURLOPT_HTTPHEADER, "Content-Type: 'application/atom+xml'");
    curl_setopt($request, CURLOPT_POST, TRUE);
    curl_setopt($request, CURLOPT_POSTFIELDS, $changed);
    curl_exec($request);
    if (!in_array(curl_getinfo($request, CURLINFO_HTTP_CODE), array(202, 204))) {
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
      $this->saveSubscription(array('topic' => $post['hub_topic'], 'subscriber' => $post['hub_callback'], 'created' => time()));
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
    if (SAVED_UPDATED != drupal_write_record('push_hub_subscriptions', $subscription, array('topic', 'subscriber'))) {
      drupal_write_record('push_hub_subscriptions', $subscription);
    }
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
      $subscribers[] = $subscriber;
    }
    return $subscribers;
  }
}