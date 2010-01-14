<?php

/**
 * Publish/Subscribe fetcher. Supports at the moment only PubSubHubbub (PuSH).
 *
 */
class FeedsPubSubFetcher extends FeedsFetcher {

  /**
   * Implementation of FeedsFetcher::fetch().
   */
  function fetch(FeedsSource $source) {
    error_log(print_r($_POST, true));
  }
}