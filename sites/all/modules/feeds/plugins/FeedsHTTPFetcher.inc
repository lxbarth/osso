<?php
// $Id: FeedsHTTPFetcher.inc,v 1.9 2010/02/18 16:01:19 alexb Exp $

/**
 * @file
 * Home of the FeedsHTTPFetcher and related classes.
 */

/**
 * Definition of the import batch object created on the fetching stage by
 * FeedsHTTPFetcher.
 */
class FeedsHTTPBatch extends FeedsImportBatch {
  protected $url;
  protected $file_path;

  /**
   * Constructor.
   */
  public function __construct($url = NULL) {
    $this->url = $url;
    parent::__construct();
  }

  /**
   * Implementation of FeedsImportBatch::getRaw();
   */
  public function getRaw() {
    feeds_include_library('http_request.inc', 'http_request');
    $result = http_request_get($this->url);
    if ($result->code != 200) {
      throw new Exception(t('Download of @url failed with code !code.', array('@url' => $this->url, '!code' => $result->code)));
    }
    return $result->data;
  }

  /**
   * Implementation of FeedsImportBatch::getFilePath().
   */
  public function getFilePath() {
    if (!isset($this->file_path)) {
      $dest = file_destination(file_directory_path() .'/feeds/'. get_class($this) .'_'. md5($this->url) .'_'. time(), FILE_EXISTS_RENAME);
      $this->file_path = file_save_data($this->getRaw(), $dest);
      if($this->file_path === 0) {
        throw new Exception(t('Cannot write content to %dest', array('%dest' => $dest)));
      }
    }
    return $this->file_path;
  }
}

/**
 * Fetches data via HTTP.
 */
class FeedsHTTPFetcher extends FeedsFetcher {

  /**
   * Implementation of FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    drupal_alter('feeds_prefetch', $source_config, $this, $source);
    $url = $source_config['source'];
    return new FeedsHTTPBatch($url);
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
}

