<?php

/**
 * Edit form callback.
 */
function openid_provider_sso_rp_edit_form() {
  if ($_GET['realm']) {
    $rp = openid_provider_sso_relying_party($_GET['realm']);
  }
  $form = array();
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => empty($rp) ? '' : $rp['name'],
  );
  $form['realm'] = array(
    '#type' => 'textfield',
    '#title' => t('Realm (URL)'),
    '#default_value' => empty($rp) ? '' : $rp['realm'],
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Remove form callback.
 */
function openid_provider_sso_rp_remove_form($form_state) {
  if ($rp = openid_provider_sso_relying_party($_GET['realm'])) {
    $form = array();
    $form['#realm'] = $rp['realm'];
    $form['#redirect'] = 'admin/settings/openid-provider-sso';
    $question = t('Remove Relying Party?');
    $description = t('If you remove the Relying Party !relying_party, it cannot use this OpenID Provider site for authentication anymore. You can add it back at any point later.', array('!relying_party' => l(check_plain($rp['name']), $rp['realm'])));
    return confirm_form($form, $question, $form['#redirect'], $description, t('Remove'));
  }
}

/**
 * Remove form submit handler.
 */
function openid_provider_sso_rp_remove_form_submit($form, &$form_state) {
  openid_provider_sso_rp_remove($form['#realm']);
}

/**
 * Admin settings page callback.
 */
function openid_provider_sso_rps_page() {
  $rps = openid_provider_sso_relying_parties();
  $rows = array(
    'value' => l(t('Add Relying Party'), 'admin/settings/openid-provider-sso/add'),
    'attributes' => array('colspan' => 3),
  );
  foreach ($rps as $rp) {
    $edit = l(t('edit'), 'admin/settings/openid-provider-sso/edit', array('query' => 'realm='. urlencode($rp['realm'])));
    $remove = l(t('remove'), 'admin/settings/openid-provider-sso/remove', array('query' => 'realm='. urlencode($rp['realm'])));
    $rows[] = array(
      l(check_plain($rp['realm']), $rp['realm']),
      check_plain($rp['name']),
      $edit .' | '. $remove,
    );
  }
  return theme_table(array(t('Realm'), t('Name')), $rows);
}
