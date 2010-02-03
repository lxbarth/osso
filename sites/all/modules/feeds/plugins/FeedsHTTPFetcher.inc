<?php
// $Id: FeedsHTTPFetcher.inc,v 1.8 2009/12/20 23:54:44 alexb Exp $

/**
 * @file
 * Home of the FeedsHTTPFetcher.
 */

/**
 * Fetches data via HTTP.
 */
class FeedsHTTPFetcher extends FeedsFetcher {

  /**
   * Implementation of FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $url = $source_config['source'];
    if (module_exists('keyauth') && $this->config['use_keyauth']) {
      keyauth_include();
      $url = keyauth_sign_url($this->config['keyauth_public'], $url);
    }
    return new FeedsImportBatch($url);
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
    return array(
      'auto_detect_feeds' => FALSE,
      'use_keyauth' => FALSE,
      'keyauth_public' => '',
    );
  }

  /**
   * Importer level configuration.
   */
  public function configForm(&$form_state) {
    $form = array();
    if (module_exists('keyauth')) {
      keyauth_include();
      drupal_add_js(drupal_get_path('module', 'feeds_ui') .'/feeds_ui.js');
      $form['use_keyauth'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use Key Authentication'),
        '#description' => t('If checked, uses Key Authentication to sign each request URL.'),
        '#default_value' => $this->config['use_keyauth'],
      );
      $form['keyauth_public'] = array(
        '#type' => 'select',
        '#title' => t('Public key'),
        '#description' => t('Public authentication key. Go to !keyauth to manage keys.', array('!keyauth' => l(t('Authentication key page'), 'admin/build/keys'))),
        '#default_value' => $this->config['keyauth_public'],
        '#options' => drupal_map_assoc(array_keys(keyauth_all_keys())),
      );
    }
    return $form;
  }
}

