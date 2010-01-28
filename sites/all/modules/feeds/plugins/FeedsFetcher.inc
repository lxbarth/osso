<?php
// $Id: FeedsFetcher.inc,v 1.4 2009/12/20 23:54:44 alexb Exp $

/**
 * Abstract class, defines shared functionality between fetchers.
 *
 * Implements FeedsSourceInfoInterface to expose source forms to Feeds.
 */
abstract class FeedsFetcher extends FeedsPlugin {

  /**
   * Fetch content from a source and return it.
   *
   * Every class that extends FeedsFetcher must implement this method.
   *
   * @param $source
   *   Source value as entered by user through sourceForm().
   */
  public abstract function fetch(FeedsSource $source);

  /**
   * Clear all caches for results for given source.
   *
   * @param FeedsSource $source
   *   Source information for this expiry. Implementers can choose to only clear
   *   caches pertaining to this source.
   */
  public function clear(FeedsSource $source) {}

  /**
   * Subscribe to a source. Only implement if fetcher requires subscription.
   *
   * @param FeedsSource $source
   *   Source information for this subscription.
   */
  public function subscribe(FeedsSource $source) {}

  /**
   * Unsubscribe from a source. Only implement if fetcher requires subscription.
   *
   * @param FeedsSource $source
   *   Source information for unsubscribing.
   */
  public function unsubscribe(FeedsSource $source) {}

  /**
   * Indicate the time within which a subscription needs to be renewed.
   */
  public function subscriptionPeriod() {
    return FEEDS_SCHEDULE_NEVER;
  }
}
