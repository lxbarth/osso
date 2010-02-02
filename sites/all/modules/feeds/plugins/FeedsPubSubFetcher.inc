<?php

feeds_include_library('PushSubscriber.inc', 'PushSubscriber');

/**
 * Publish/Subscribe fetcher. Supports at the moment only PubSubHubbub (PuSH).
 */
class FeedsPubSubFetcher extends FeedsHTTPFetcher {

  /**
   * Implementation of FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    // Handle fat ping if present, otherwise pass up to HTTP fetcher.
    if ($raw = feeds_push_subscriber($this->id, $source->feed_nid)->receive()) {
      return new FeedsImportBatch($source_config['source'], NULL, $raw);
    }
    return new FeedsImportBatch($source_config['source']);
  }

  /**
   * Override sourceSave() - subscribe to hub.
   */
  public function sourceSave(FeedsSource $source) {
    $this->subscribe($source);
  }

  /**
   * Implement FeedsFetcher::subscribe() - subscribe to hub.
   */
  public function subscribe(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    feeds_push_subscriber($this->id, $source->feed_nid)->subscribe($source_config['source'], url('feeds/pubsubhubbub/notify/'. $this->id .'/'. $source->feed_nid, array('absolute' => TRUE)));
  }

  /**
   * Implement FeedsFetcher::unsubscribe() - unsubscribe from hub.
   */
  public function unsubscribe(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    feeds_push_subscriber($this->id, $source->feed_nid)->unsubscribe($source_config['source'], url('feeds/pubsubhubbub/notify/'. $this->id .'/'. $source->feed_nid, array('absolute' => TRUE)));
  }

  /**
   * Implement FeedsFetcher::subscriptionPeriod().
   * Indicate how often a subscription needs to be renewed.
   *
   * @todo subscription_period should actually be retrieved from the hub's
   *   response to a subscription. This will mean a different subscription
   *   period per source, hence a major change to FeedsScheduler which is
   *   currently assuming a fixed period per task!
   */
  public function subscriptionPeriod() {
    return $this->config['subscription_period'];
  }

  /**
   * Return defaults for configuration.
   */
  public function configDefaults() {
    return array(
      'subscription_period' => 3600*24, // Renew subscription in 24 hours.
    );
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $form = array();
    $period = drupal_map_assoc(array(0, 900, 1800, 3600, 10800, 21600, 43200, 86400, 259200, 604800, 2419200), 'format_interval');
    $period[FEEDS_SCHEDULE_NEVER] = t('Never renew');
    $period[0] = t('Renew as often as possible');
    $form['subscription_period'] = array(
      '#type' => 'select',
      '#title' => t('Renew subscription after'),
      '#options' => $period,
      '#description' => t('This is the minimum time that must elapse before a subscription is renewed.'),
      '#default_value' => $this->config['subscription_period'],
    );
    return $form;
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
 *
 * @return PushSubscriber
 *   A PushSubscriber object.
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
