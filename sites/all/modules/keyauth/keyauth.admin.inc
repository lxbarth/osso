<?php
/**
 * @file
 *  Generate security keys.
 */

/**
 * List available keys.
 */
function keyauth_admin_keys_list() {
  $rows = array();
  keyauth_include();
  foreach (keyauth_all_keys() as $key) {
    unset($delete);
    $row = array();
    $row[] = $key['title'];
    $row[] = $key['public_key'];
    $row[] = $key['private_key'];
    if ($key['export_type'] == EXPORT_IN_CODE) {
      $status = t('default');
      $edit = t('override');
    }
    elseif ($key['export_type'] == EXPORT_IN_DATABASE) {
      $status = t('normal');
      $edit = t('edit');
      $delete = t('delete');
    }
    else {
      $status = t('overridden');
      $edit = t('edit');
      $delete = t('revert');
    }
    $row[] = $status;
    $row[] = l($edit, 'admin/build/keys/'. $key['public_key']);
    $row[] =  l(t('generate'), 'admin/build/keys/'. $key['public_key'] .'/generate');
    if ($delete) {
      $row[] = l($delete, 'admin/build/keys/'. $key['public_key'] .'/delete');
    }
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No API keys created.'), 'colspan' => '5', 'class' => 'message'));
  }
  $header = array(t('Title'), t('Public key'), t('Private key'), t('Status'), array('data' => t('Operations'), 'colspan' => '3'));
  return theme('table', $header, $rows);
}

/**
 * Create/edit keys.
 */
function keyauth_admin_keys_form($form_state, $arg = '') {
  keyauth_include();
  $key = keyauth_load($arg);

  $form = array();
  $form['#public_key'] = isset($key['public_key']) ? $key['public_key'] : '';
  $form['#redirect'] = 'admin/build/keys';
  $form['title'] = array(
    '#title'          => t('Application title'),
    '#type'           => 'textfield',
    '#default_value'  => isset($key['title']) ? $key['title'] : '',
    '#description'    => t('The title of the application or website using the service.'),
    '#required'       => TRUE,
    '#weight'         => -1,
  );
  if ($arg != 'new') {
    $form['public_key'] = array(
      '#title'          => t('Public key'),
      '#type'           => 'textfield',
      '#default_value'  => isset($key['public_key']) ? $key['public_key'] : '',
      '#description'    => t('Public key for the service.'),
      '#required'       => TRUE,
    );
    $form['private_key'] = array(
      '#title'          => t('Private key'),
      '#type'           => 'textfield',
      '#default_value'  => isset($key['private_key']) ? $key['private_key'] : '',
      '#description'    => t('Private key for the service. This key is never sent over the network.'),
      '#required'       => TRUE,
    );
  }
  $form['submit'] = array(
    '#type'           => 'submit',
  );
  if ($arg == 'new') {
    $form['submit']['#value'] = t('Create key');
    $form['#submit'] = array('keyauth_admin_keys_form_submit_create');
  }
  elseif ($arg == 'add') {
    $form['submit']['#value'] = t('Add key');
    $form['#validate'] = array('keyauth_admin_keys_form_validate_public_key');
    $form['#submit'] = array('keyauth_admin_keys_form_submit_save');
  }
  else {
    $form['submit']['#value'] = t('Save key');
    $form['#submit'] = array('keyauth_admin_keys_form_submit_save');
  }
  return $form;
}

/**
 * Validation handler for keyauth_admin_keys_form().
 */
function keyauth_admin_keys_form_validate_public_key($form, &$form_state) {
  keyauth_include();
  if (keyauth_load($form_state['values']['public_key'])) {
    form_set_error('public_key', t('Public key exists already.'));
  }
}

/**
 * Submit handler for keyauth_admin_keys_form().
 */
function keyauth_admin_keys_form_submit_create($form, &$form_state) {
  keyauth_include();
  $key = keyauth_save($form_state['values']['title']);
  drupal_set_message(t('Key @key has been created.', array('@key' => $key['public_key'])));
}

/**
 * Submit handler for keyauth_admin_keys_form().
 */
function keyauth_admin_keys_form_submit_save($form, &$form_state) {
  keyauth_include();
  $key = keyauth_save($form_state['values']);
  drupal_set_message(t('Key @key has been saved.', array('@key' => $key['public_key'])));
}

/**
 * Delete confirmation.
 */
function keyauth_admin_keys_delete_confirm(&$form_state, $public_key) {
  $form['#public_key'] = $public_key;
  $form['#redirect'] = 'admin/build/keys';
  $message = t('Delete the key @key?', array('@key' => $public_key));
  $caption = '';
  $caption .= '<p>'. t('This action cannot be undone.') .'</p>';
  return confirm_form($form, $message, $form['#redirect'], $caption, t('Delete'));
}

/**
 * Submit handler for keyauth_admin_keys_delete_confirm().
 */
function keyauth_admin_keys_delete_confirm_submit($form, &$form_state) {
  keyauth_include();
  keyauth_delete($form['#public_key']);
  drupal_set_message(t('The key @key has been deleted.', array('@key' => $form['#public_key'])));
  watchdog('menu', 'Deleted key @key.', array('@key' => $form['#public_key']), WATCHDOG_NOTICE);
}

/**
 * Generate confirmation.
 */
function keyauth_admin_keys_generate_confirm(&$form_state, $public_key) {
  $form['#public_key'] = $public_key;
  $form['#redirect'] = 'admin/build/keys';
  $message = t('Generate a new <em>private key</em> for @key?', array('@key' => $public_key));
  $caption = '';
  $caption .= '<p>'. t('This action cannot be undone.') .'</p>';
  return confirm_form($form, $message, $form['#redirect'], $caption, t('Generate new private key'));
}

/**
 * Submit handler for keyauth_admin_keys_generate_confirm().
 */
function keyauth_admin_keys_generate_confirm_submit($form, &$form_state) {
  keyauth_include();
  $keyset = keyauth_regenerate($form['#public_key']);
  drupal_set_message(t('Your new private key is @key', array('@key' => $keyset['private_key'])));
}
