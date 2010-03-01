<?php

/**
 * @file
 * PubsubHubbub library.
 */

// TTL of a subscription.
define('PUSH_HUB_LEASE_SECONDS', 3600*48);

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
   * @param $topic
   *   The topic that changed.
   * @param $subscriber
   *   A subscriber callback.
   * @param $changed
   *   The full or partial feed that contains the changed elements. If NULL,
   *   a light ping (without any content) will be issued to subscriber and
   *   subscriber is expected to fetch content from publisher. Light pings are
   *   not pubsubhubbub spec conform.
   *
   * @return
   *   TRUE if subscriber was successfully notified, FALSE otherwise.
   */
  public function notify($topic, $subscriber, $changed = NULL) {
    if ($changed === NULL) {
      return $this->lightPing($subscriber);
    }
    return $this->fatPing($subscriber, $changed, $this->subscriptions->secret($topic, $subscriber));
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
      'hub.lease_seconds='. PUSH_HUB_LEASE_SECONDS, // disregard what's been requested ($post['hub_lease_seconds'])
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
    error_log(print_r($post, true));
    if (isset($post['hub_topic']) && isset($post['hub_callback']) && $this->verify($post)) {
      $this->subscriptions->save($post['hub_topic'], $post['hub_callback'], isset($post['secret']) ? $post['secret'] : '');
      header('HTTP/1.1 204 "No Content"', null, 204);
      exit();
    }
    header('HTTP/1.1 404 "Not Found"', null, 404);
    exit();
  }

  /**
   * Expire old subscriptions.
   */
  public function expire() {
    $this->subscriptions->expire(PUSH_HUB_LEASE_SECONDS);
  }

  /**
   * Helper for posting a fat ping. Uses stream wrappers instead of cURL for
   * posting a message body that is not "application/x-www-form-urlencoded".
   */
  protected function fatPing($url, $content, $secret) {
    $result = FALSE;
    $params = array('http' =>
                array(
                  'method' => 'POST',
                  'content' => $content,
                  'header' => "Content-Type: application/atom+xml\n".
                              "Content-Length: ". strlen($content) ."\n\n",
                ),
              );
    if (!empty($secret)) {
      $params['http']['header']['X-Hub-Signature'] = hash_hmac('sha1', $content, $secret);
    }
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
   * Get the secret for a given topic/subscriber pair.
   *
   * @return
   *   Shared secret to sign a notification with.
   */
  public function secret($topic, $subscriber);

  /**
   * Save a subscription.
   *
   * @param $topic
   *   A topic URL.
   * @param $subscriber
   *   The subscriber URL that is the callback to be invoked when the topic
   *   changes.
   * @param $secret
   *   Secret for message authentication.
   */
  public function save($topic, $subscriber, $secret);

  /**
   * Delete a subscription.
   *
   * @param $topic
   *   A topic URL.
   * @param $subscriber
   *   The subscriber URL that is the callback to be invoked when the topic
   *   changes.
   */
  public function delete($topic, $subscriber);

  /**
   * Find all subscriber URLs for a given topic URL.
   *
   * @param $topic
   *   A topic URL.
   *
   * @return
   *   An array of subscriber URLs.
   */
  public function all($topic);

  /**
   * Expire subscriptions older than $time.
   *
   * @param $time
   *   Subscriptions older than time() - $time will be deleted.
   */
  public function expire($time);
}
