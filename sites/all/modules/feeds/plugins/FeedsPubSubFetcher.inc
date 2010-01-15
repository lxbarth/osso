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
  public function sourceSave(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    feeds_push_subscriber($this->id, $source->feed_nid)->subscribe($source_config['source']);
  }

  /**
   * Verify a PubSubHubbub subscription request.
   *
   * @param unknown_type $verify_fields
   */
  public function pushVerifySubscriptionRequest($verify_fields, $feed_nid) {
    feeds_push_subscriber($this->id, $feed_nid)->verifySubscriptionRequest($verify_fields);
  }

  /**
   * Receive a PubSubHubbub notification.
   */
  protected function pushReceive() {
    // error_log(print_r($_POST, true));
  }
}

/**
 * Create a PubSubHubbub subscriber.
 */
function feeds_push_subscriber($id, $subscriber_id) {
  feeds_include_library('PushSubscriber.inc', 'PushSubscriber');
  return PushSubscriber::instance($id, $subscriber_id);
}