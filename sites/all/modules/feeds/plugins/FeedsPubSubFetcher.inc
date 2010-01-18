<?php

feeds_include_library('PushSubscriber.inc', 'PushSubscriber');

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
    if ($raw = feeds_push_subscriber($this->id, $source->feed_nid)->receive($_POST)) {
      return new FeedsImportBatch($source_config['source'], NULL, $raw);
    }
    return new FeedsImportBatch($source_config['source']);
  }

  /**
   * Override sourceSave() - subscribe to hub.
   */
  public function sourceSave(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    feeds_push_subscriber($this->id, $source->feed_nid)->subscribe($source_config['source'], url('feeds/pubsubhubbub/notify/'. $this->id .'/'. $source->feed_nid, array('absolute' => TRUE)));
  }

  /**
   * Verify a PubSubHubbub subscription request.
   */
  public function pushVerifySubscriptionRequest($verify_fields, $feed_nid) {
    feeds_push_subscriber($this->id, $feed_nid)->verifySubscriptionRequest($verify_fields);
  }
}

/**
 * Create a PubSubHubbub subscriber.
 */
function feeds_push_subscriber($id, $subscriber_id) {
  return PushSubscriber::instance($id, $subscriber_id, PuSHSubscriptions::instance());
}

/**
 * Implement a PuSHSubscriptionsInterface.
 */
class PuSHSubscriptions implements PuSHSubscriptionsInterface {
  /**
   * Singleton.
   */
  public function instance() {
    static $subscriptions;
    if (empty($subscriptions)) {
      $subscriptions = new PuSHSubscriptions();
    }
    return $subscriptions;
  }

  /**
   * Protect constructor.
   */
  protected function __construct() {
  }

  /**
   * Save a subscription.
   */
  public function save($domain, $subscriber_id, $hub, $topic, $mode, $post_fields = array()) {
    $save = array(
      'domain' => $domain,
      'subscriber_id' => $subscriber_id,
      'hub' => $hub,
      'topic' => $topic,
      'mode' => $mode,
      'post_fields' => $post_fields,
      'timestamp' => time(),
    );
    $this->delete($domain, $subscriber_id);
    drupal_write_record('feeds_push_subscriptions', $save);
  }

  /**
   * Load a subscription.
   */
  public function load($domain, $subscriber_id) {
    if ($request = db_fetch_array(db_query("SELECT * FROM {feeds_push_subscriptions} WHERE domain = '%s' AND subscriber_id = %d", $domain, $subscriber_id))) {
      $request['post_fields'] = unserialize($request['post_fields']);
    }
    return $request;
  }

  /**
   * Delete a subscription.
   */
  public function delete($domain, $subscriber_id) {
    db_query("DELETE FROM {feeds_push_subscriptions} WHERE domain = '%s' AND subscriber_id = %d", $domain, $subscriber_id);
  }
}
