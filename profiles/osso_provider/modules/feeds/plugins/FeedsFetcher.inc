<?php
// $Id: FeedsFetcher.inc,v 1.1 2009/10/20 21:03:08 alexb Exp $

/**
 * Defines the object a Fetcher returns on fetch().
 */
class FeedsFetcherResult extends FeedsResult {
  // Define valid types.
  // @todo: does text/filepath make sense?
  // @todo: If convenient, we could expand on this concept and build content
  //   type negotiation between Fetchers and Parsers.
  protected $valid_types = array('text/filepath', 'text/xml');
}

/**
 * Abstract class, defines shared functionality between fetchers.
 *
 * Implements FeedsSourceInfoInterface to expose source forms to Feeds.
 */
abstract class FeedsFetcher extends FeedsPlugin {

  /**
   * Fetch content from a source and return it.
   *
   * Stub method. Every class that extends FeedsFetcher must implement this method.
   *
   * @param $source
   *   Source value as entered by user through sourceForm().
   *
   * @todo: Define format of return value?
   * @todo: Should it be execute()?
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
}
