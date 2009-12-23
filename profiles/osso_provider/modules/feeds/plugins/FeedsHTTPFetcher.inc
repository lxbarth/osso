<?php
// $Id: FeedsHTTPFetcher.inc,v 1.5 2009/11/17 20:14:30 alexb Exp $

/**
 * @file
 * Home of the FeedsHTTPFetcher.
 */

/**
 * Fetches data via HTTP.
 */
class FeedsHTTPFetcher extends FeedsFetcher {

  /**
   * Fetch a resource via http.
   *
   * @param $resource
   *   A resource description of type FeedsResource.
   *
   * @return
   *   A string from the requested location if successful, or FALSE if not.
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $url = $source_config['source'];

    feeds_include_library('http_request.inc', 'http_request');
    if ($this->config['auto_detect_feeds']) {
      $result = http_request_get_common_syndication($url);
    }
    else {
      $result = http_request_get($url);
    }
    if ($result->code != 200) {
      throw new Exception(t('Download of @url failed with code !code.', array('@url' => $url, '!code' => $result->code)));
    }
    return new FeedsFetcherResult($result->data, 'text/xml');
  }

  /**
   * Clear caches.
   */
  public function clear(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $url = $source_config['source'];
    feeds_include_library('http_request.inc', 'http_request');
    http_request_clear_cache($url);
  }

  /**
   * Expose source form.
   */
  public function sourceForm($source_config) {
    $form = array();
    $form['source'] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#description' => t('Enter a feed URL.'),
      '#default_value' => isset($source_config['source']) ? $source_config['source'] : '',
      '#maxlength' => NULL,
      '#required' => TRUE,
    );
    return $form;
  }

  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    return array('auto_detect_feeds' => FALSE);
  }

  /**
   * Configuration form.
   */
  public function configForm(&$form_state) {
    $form = array();
    return $form;
    // @todo:
    $form['auto_detect_feeds'] = array(
      '#type' => 'checkbox',
      '#title' => t('Auto detect feeds (not tested)'),
      '#description' => t('Auto detect RSS or Atom feeds in the downloaded document and use them instead of the given URL.'),
      '#default_value' => $this->config['auto_detect_feeds'],
      );
    return $form;
  }
}

