<?php

/**
 * @file
 * A PubsubHubbub Hub.
 */

class PuSHHub {

  /**
   * Notify subscribers of a change in topic.
   *
   * @param $topic
   *   URL of the topic that changed.
   * @param $changed
   *   The feed that contains the changed elements.
   */
  function notify($topic, $changed) {
  }

  /**
   * Verify subscription request.
   */
  function verify($post) {
    // Send verification request to subscriber.
    $challenge = md5(session_id() . rand());
    $query = array(
      'hub.mode='. $post['hub_mode'],
      'hub.topic='. $post['hub_topic'],
      'hub.challenge='. $challenge,
      'hub.lease_seconds='. $post['hub_lease_seconds'],
      'hub.verify_token='. $post['hub_verify_token'],
    );
    $parsed = parse_url($post['hub_callback']);
    $request = curl_init($post['hub_callback'] . (empty($parsed['query']) ? '?' : '&') . implode('&', $query));
    curl_setopt($request, CURLOPT_FOLLOWLOCATION, TRUE);
    curl_setopt($request, CURLOPT_RETURNTRANSFER, TRUE);
    $data = curl_exec($request);
    $data = substr($data, curl_getinfo($request, CURLINFO_HEADER_SIZE));
    $code = curl_getinfo($request, CURLINFO_HTTP_CODE);
    curl_close($request);
    if ($code > 199 && $code > 300 && $data == $challenge) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Save a notification subscription.
   *
   * @param $suscriber
   *   A callback URL of a subscriber.
   * @param $topic
   *   The topic of which changes a subscriber would like to notified of.
   */
  function saveSubscription($subscriber, $topic) {

  }

  /**
   * Delete subscription.
   * @param $suscriber
   *   A callback URL of a subscriber.
   * @param $topic
   *   The topic of which changes a subscriber would like to notified of.
   */
  function deleteSubscription($subscriber, $topic) {
  }
}