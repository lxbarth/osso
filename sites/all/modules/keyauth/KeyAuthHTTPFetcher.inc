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
    return new FeedsHTTPBatch($this->signURL($source_config['source']));
  }

  /**
   * Override subscribe method to use key authentication as well.
   */
  public function subscribe(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $this->subscriber($source->feed_nid)->subscribe($this->signURL($source_config['source']), url($this->path($source->feed_nid), array('absolute' => TRUE)), valid_url($this->config['designated_hub']) ? $this->config['designated_hub'] : '');
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

  /**
   * Helper for signing a URL. Uses KeyAuth to sign a URL if the fetcher is
   * configured to do so, otherwise returns the URL exactly as supplied.
   */
  protected function signURL($url) {
    if ($this->config['use_keyauth']) {
      keyauth_include();
      return keyauth_sign_url($this->config['keyauth_public'], $url);
    }
    return $url;
  }
}
