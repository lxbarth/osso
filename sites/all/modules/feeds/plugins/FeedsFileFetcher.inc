<?php
// $Id: FeedsFileFetcher.inc,v 1.5 2010/01/18 20:05:53 alexb Exp $

/**
 * @file
 * Home of the FeedsFileFetcher.
 */

/**
 * Fetches data via HTTP.
 */
class FeedsFileFetcher extends FeedsFetcher {

  /**
   * Implementation of FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    return new FeedsImportBatch(NULL, $source_config['source']);
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