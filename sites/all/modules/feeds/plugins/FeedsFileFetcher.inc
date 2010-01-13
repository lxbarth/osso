<?php
// $Id: FeedsFileFetcher.inc,v 1.2 2009/10/20 20:59:04 alexb Exp $

/**
 * @file
 * Home of the FeedsFileFetcher.
 */

/**
 * Fetches data via HTTP.
 */
class FeedsFileFetcher extends FeedsFetcher {

  /**
   * Fetch a local resource.
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    // Just return path to file, contents can be read easily with
    // file_get_contents($file_path);
    return new FeedsFetcherResult($source_config['source'], 'text/filepath');
  }

  /**
   * Source form.
   */
  public function sourceForm($source_config) {
    $form = array();
    // When renaming, do not forget feeds_vews_handler_field_source class.
    $form['source'] = array(
      '#type' => 'textfield',
      '#title' => t('File'),
      '#description' => t('Specify a file in the site\'s file system path or upload a file below.'),
      '#default_value' => isset($source_config['source']) ? $source_config['source'] : '',
    );
    $form['upload'] = array(
      '#type' => 'file',
      '#title' => t('Upload'),
      '#description' => t('Choose a file from your local computer, then click "Import".'),
    );
    return $form;
  }

  /**
   * Override parent::sourceFormValidate().
   */
  public function sourceFormValidate(&$values) {
    $feed_dir = file_directory_path() .'/feeds';
    file_check_directory($feed_dir, TRUE);

    // If there is a file uploaded, save it, otherwise validate input on
    // file.
    if ($file = file_save_upload('feeds', array(), $feed_dir)) {
      file_set_status($file, FILE_STATUS_PERMANENT);
      $values['source'] = $file->filepath;
    }
    elseif (empty($values['source'])) {
      form_set_error('feeds][source', t('Upload a file first.'));
    }
    elseif (!file_check_location($values['source'], file_directory_path())) {
      form_set_error('feeds][source', t('File needs to point to a file in your Drupal file system path.'));
    }
  }
}