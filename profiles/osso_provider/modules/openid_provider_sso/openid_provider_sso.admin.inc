<?php

/**
 * Edit form callback.
 */
function openid_provider_sso_rp_edit_form($realm = NULL) {
  $form['relying_party'] = array(
    '#type' => 'fieldset',
    '#title' => 'Add new Relying Party',
    '#description' => 'Add a Relying Party with OpenID Simple Sign-On support - a Drupal site with OpenID and OpenID SSO module installed and configured.',
  );
  $form['relying_party']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
  );
  $form['relying_party']['realm'] = array(
    '#type' => 'textfield',
    '#title' => t('Realm (URL)'),
  );
  $form['relying_party']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
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
 * Admin settings form callback.
 */
function openid_provider_sso_rps_form() {
  $rps = openid_provider_sso_relying_parties();
  $form = array();
  $form['existing']['#tree'] = TRUE;
  foreach ($rps as $rp) {
    $form['existing'][$rp['realm']]['realm']['#value'] =$rp['realm'];
    $form['existing'][$rp['realm']]['name']['#value'] =$rp['name'];
    $edit = l(t('edit'), 'admin/settings/openid-provider-sso/edit', array('query' => 'realm='. urlencode($rp['realm'])));
    $remove = l(t('remove'), 'admin/settings/openid-provider-sso/remove', array('query' => 'realm='. urlencode($rp['realm'])));
    $form['existing'][$rp['realm']]['operations']['#value'] = $edit .' | '. $remove;
  }
  return $form;
}

