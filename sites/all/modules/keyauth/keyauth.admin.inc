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
  $result = db_query("SELECT * FROM {keyauth_keys}");
  while ($key = db_fetch_object($result)) {
    $row = array();
    $row[] = $key->title;
    $row[] = $key->public_key;
    $row[] = $key->private_key;

    // Populate the operations field.
    $operations = array();
    $operations[] = array('data' => l(t('edit'), 'admin/build/keys/'. $key->public_key));
    $operations[] = array('data' => l(t('generate'), 'admin/build/keys/'. $key->public_key .'/generate'));
    $operations[] = array('data' => l(t('delete'), 'admin/build/keys/'. $key->public_key .'/delete'));
    foreach ($operations as $operation) {
      $row[] = $operation;
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No API keys created.'), 'colspan' => '5', 'class' => 'message'));
  }
  $header = array(t('Title'), t('Public key'), t('Private key'), array('data' => t('Operations'), 'colspan' => '2'));
  return theme('table', $header, $rows);
}

/**
 * Create/edit keys.
 */
function keyauth_admin_keys_form($form_state, $arg = '') {
  $key = db_fetch_object(db_query("SELECT * FROM {keyauth_keys} WHERE public_key = '%s'", $arg));

  $form = array();
  $form['#public_key'] = isset($key->public_key) ? $key->public_key : '';
  $form['#redirect'] = 'admin/build/keys';
  $form['title'] = array(
    '#title'          => t('Application title'),
    '#type'           => 'textfield',
    '#default_value'  => isset($key->title) ? $key->title : '',
    '#description'    => t('The title of the application or website using the service.'),
    '#required'       => TRUE,
    '#weight'         => -1,
  );
  if ($arg != 'new') {
    $form['public_key'] = array(
      '#title'          => t('Public key'),
      '#type'           => 'textfield',
      '#default_value'  => isset($key->public_key) ? $key->public_key : '',
      '#description'    => t('Public key for the service.'),
      '#required'       => TRUE,
    );
    $form['private_key'] = array(
      '#title'          => t('Private key'),
      '#type'           => 'textfield',
      '#default_value'  => isset($key->private_key) ? $key->private_key : '',
      '#description'    => t('Private key for the service. This key is never sent over the network.'),
      '#required'       => TRUE,
    );
  }
  if ($arg == 'new') {
    $value = t('Create key');
  }
  elseif ($arg == 'add') {
    $value = t('Add key');
  }
  else {
    $value = t('Save key');
  }
  $form['submit'] = array(
    '#type'           => 'submit',
    '#value'          => $value,
  );
  return $form;
}

/**
 * Validation handler for keyauth_admin_keys_form().
 */
function keyauth_admin_keys_form_validate($form, &$form_state) {
  if (!empty($form_state['values']['public_key']) && $form['#public_key'] != $form_state['values']['public_key'] && db_result(db_query("SELECT public_key FROM {keyauth_keys} WHERE public_key = '%s'", $form_state['values']['public_key']))) {
    form_set_error('public_key', t('Public key exists already.'));
  }
}

/**
 * Submit handler for keyauth_admin_keys_form().
 */
function keyauth_admin_keys_form_submit($form, &$form_state) {
  keyauth_include();
  if (empty($form['#public_key']) && empty($form_state['values']['public_key'])) {
    keyauth_insert($form_state['values']['title']);
  }
  elseif (empty($form['#public_key']) && !empty($form_state['values']['public_key'])) {
    keyauth_insert($form_state['values']['title'], $form_state['values']['public_key'], $form_state['values']['private_key']);
  }
  elseif (!empty($form['#public_key'])) {
    keyauth_update($form['#public_key'], $form_state['values']['title'], $form_state['values']['private_key']);
  }
}

/**
 * Delete a key.
 */
function keyauth_admin_keys_delete($public_key) {
  db_query("DELETE FROM {keyauth_keys} WHERE public_key = '%s'", $public_key);
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
  keyauth_admin_keys_delete($form['#public_key']);
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
