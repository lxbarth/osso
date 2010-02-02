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
    $operations[] = array('data' => l(t('regenerate'), 'admin/build/keys/'. $key->public_key .'/regenerate'));
    $operations[] = array('data' => l(t('delete'), 'admin/build/keys/'. $key->public_key .'/delete'));
    foreach ($operations as $operation) {
      $row[] = $operation;
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No API keys created.'), 'colspan' => '5', 'class' => 'message'));
  }
  $header = array(t('Key'), t('Title'), t('Domain'), array('data' => t('Operations'), 'colspan' => '2'));
  return theme('table', $header, $rows);
}

/**
 * Create/edit keys.
 */
function keyauth_admin_keys_form($form_state, $public_key = '') {
  $key = db_fetch_object(db_query("SELECT * FROM {keyauth_keys} WHERE public_key = '%s'", $public_key));

  $form = array();
  $form['#public_key'] = isset($key->public_key) ? $key->public_key : '';
  $form['#redirect'] = 'admin/build/keys';
  $form['title'] = array(
    '#title'          => t('Application title'),
    '#type'           => 'textfield',
    '#default_value'  => isset($key->title) ? $key->title : '',
    '#description'    => t('The title of the application or website using the service.'),
    '#required'       => TRUE,
  );
  $form['submit'] = array(
    '#type'           => 'submit',
    '#value'          => isset($key->title) ? t('Save key') : t('Create key'),
  );
  return $form;
}

/**
 * Submit handler for keyauth_admin_keys_form().
 */
function keyauth_admin_keys_form_submit($form, &$form_state) {
  keyauth_admin_keys_save($form_state['values']['title'], $form['#public_key']);
}

/**
 * Save a key, generate a pair of keys if new.
 */
function keyauth_admin_keys_save($title, $public_key = '') {
  keyauth_include();
  if (empty($public_key)) {
    $save = array(
      'title' => $title,
      'public_key' => keyauth_random('md5'),
      'private_key' => keyauth_random('sha1'),
    );
    drupal_write_record('keyauth_keys', $save);
  }
  else {
    $save = array(
      'title' => $title,
      'public_key' => $public_key,
    );
    drupal_write_record('keyauth_keys', $save, 'public_key');
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
function keyauth_admin_keys_delete_confirm(&$form_state, $public_key = '') {
  $form['#public_key'] = $public_key;
  $form['#redirect'] = 'admin/build/keys';
  $message = t('Are you sure you would like to delete the key @key?', array('@key' => $public_key));
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
