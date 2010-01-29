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
   * Get all subscribers for a given topic.
   */
  public function allSubscribers($topic) {
    return $this->subscriptions->all($topic);
  }

  /**
   * Notify a specific subscriber of a change in a topic. API user is
   * responsible for handling failed requests.
   *
   * @param $subscriber
   *   A subscriber callback.
   * @param $topic
   *   URL of the topic that changed.
   * @param $changed
   *   The full or partial feed that contains the changed elements. If NULL,
   *   a light ping (without any content) will be issued to subscriber and
   *   subscriber is expected to fetch content from publisher. Light pings are
   *   not pubsubhubbub spec conform.
   *
   * @return
   *   TRUE if subscriber was successfully notified, FALSE otherwise.
   */
  public function notify($subscriber, $changed = NULL) {
    if ($changed === NULL) {
      return $this->lightPing($subscriber);
    }
    return $this->fatPing($subscriber, $changed);
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
      header('HTTP/1.1 204 "No Content"', null, 204);
      exit();
    }
    header('HTTP/1.1 404 "Not Found"', null, 404);
    exit();
  }

  /**
   * Helper for posting a fat ping. Uses stream wrappers instead of cURL for
   * posting non application/x-www-form-urlencoded content
   */
  protected function fatPing($url, $content) {
    $result = FALSE;
    $params = array('http' =>
                array(
                  'method' => 'POST',
                  'content' => $content,
                  'header' => "Content-Type: application/atom+xml\n".
                              "Content-Length: ". strlen($content) ."\n\n",
                ),
              );
    $ctx = stream_context_create($params);
    if ($fp = @fopen($url, 'rb', false, $ctx)) {
      $response = @stream_get_contents($fp);
      $meta = stream_get_meta_data($fp);
      preg_match('/HTTP.*?\s(\d*?)\s.*/i', $meta['wrapper_data'][0], $matches);
      $code = (integer) $matches[1];
      if ($code >= 200 && $code < 300) {
        $result = TRUE;
      }
      fclose($fp);
    }
    return $result;
  }

  /**
   * Issue a light ping, not spec conform.
   */
  protected function lightPing($url) {
    $request = curl_init($url);
    curl_setopt($request, CURLOPT_FOLLOWLOCATION, TRUE);
    $data = curl_exec($request);
    $code = curl_getinfo($request, CURLINFO_HTTP_CODE);
    curl_close($request);
    if ($code >= 200 && $code < 300) {
      return TRUE;
    }
    return FALSE;
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
