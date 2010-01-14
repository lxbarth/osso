<?php

/**
 * Publish/Subscribe fetcher. Supports at the moment only PubSubHubbub (PuSH).
 *
 */
class FeedsPubSubFetcher extends FeedsHTTPFetcher {

  /**
   * Implementation of FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    // Handle fat ping if present, otherwise pass up to HTTP fetcher.
    if ($raw = $this->pushReceive()) {
      return new FeedsImportBatch($source_config['source'], NULL, $raw);
    }
    return new FeedsImportBatch($source_config['source']);
  }

  /**
   * Override sourceSave() - subscribe to hub.
   */
  public function sourceSave(&$source_config) {
    $this->pushSubscribe($source_config['source']);
  }

  /**
   * Subscribe to a PubSubHubbub hub by URL of feed.
   *
   * @param $url
   *   The URL of the feed to subscribe to.
   *
   * @todo RSS support.
   */
  protected function pushSubscribe($url) {
    // Fetch document, find rel=hub and rel=self.
    // If present, issue subscription request.
    feeds_include_library('http_request.inc', 'http_request');
    if (!http_request_use_curl()) {
      throw new Exception(t('CURL required.'));
    }
    $result = http_request_get($url);
    if ($result->code == 200) {
      $xml = new SimpleXMLElement($raw);
      $hub = $xml->xpath("/feed/link[attribute::rel='hub']");
      $self = $xml->xpath("/feed/link[attribute::rel='self']");
      if ($hub && $self) {
        $this->pushRequest($hub, $self, 'subscribe');
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
  protected function pushRequest($hub, $topic, $mode) {
    $values = array(
      'hub.callback' => url('feeds/pubsubhubbub/notify/'. $this->id, array('absolute' => TRUE)),
      'hub.mode' => $mode,
      'hub.topic' => $topic,
      'hub.verify' => 'sync',
      'hub.lease_seconds' => '', // Permanent subscription.
      'hub.secret' => '', // @todo
      'hub.verify_token' => drupal_get_token(),
    );
    $request = curl_init($hub);
    curl_setopt($request, CURLOPT_POST, TRUE);
    curl_setopt($request, CURLOPT_POSTFIELDS, $values);
    curl_exec($request);
    if (!in_array(curl_getinfo($request), array(202, 204))) {
      drupal_set_message(t('Error subscribing to PubsubHubbub hub.'), 'error');
    }
    curl_close($request);
  }

  /**
   * Receive a PubSubHubbub notification.
   */
  protected function pushReceive() {
    error_log(print_r($_POST, true));
  }
}