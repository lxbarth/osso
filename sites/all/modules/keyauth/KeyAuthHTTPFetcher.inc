<?php

/**
 * Support signed URLs.
 */
class KeyAuthHTTPFetcher extends FeedsHTTPFetcher {
  /**
   * Use signed URL to fetch content.
   *
   * @todo: Move URL signature elements to the HTTP header?
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $url = $source_config['source'];
    if ($this->config['use_keyauth']) {
      keyauth_include();
      $url = keyauth_sign_url($this->config['keyauth_public'], $url);
    }
    return new FeedsHTTPBatch($url);
  }

  /**
   * Declare defaults.
   */
  public function configDefaults() {
    return array(
      'use_keyauth' => FALSE,
      'keyauth_public' => '',
    ) + parent::configDefaults();
  }

  /**
   * Add form options.
   */
  public function configForm(&$form_state) {
    $form = parent::configForm($form_state);
    drupal_add_js(drupal_get_path('module', 'keyauth') .'/keyauth.js');
    keyauth_include();
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
    return $form;
  }
}
