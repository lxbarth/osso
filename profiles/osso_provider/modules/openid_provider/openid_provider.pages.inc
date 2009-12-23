<?php
// $Id: openid_provider.pages.inc,v 1.1.2.3 2009/03/11 18:15:02 walkah Exp $

/**
 * @file
 * Page callbacks for OpenID Provider.
 */

/**
 * Primary endpoint callback - serves op_endpoint.
 *
 * @param array $request (Optional) request parameters.
 */
function openid_provider_endpoint($request = array()) {
  module_load_include('inc', 'openid');
  module_load_include('inc', 'openid_provider');

  if (count($request) == 0) {
    $request = _openid_response();
  }
  
  if (isset($request['openid.mode'])) {
    switch ($request['openid.mode']) {
      case 'associate':
        openid_provider_association_response($request);
        return;
      case 'checkid_immediate':
      case 'checkid_setup':
        return openid_provider_authentication_response($request);
      case 'check_authentication':
        openid_provider_verification_response($request);
        break;
    }
  }
}

/**
 * Callback page for OpenID Identifier. This page is primarily used for
 * discovery when someone is logging in from a Relying Party.
 */
function openid_provider_page($account) {
  return t('This is the OpenID page for %user.', array('%user' => $account->name));
}

/**
 * Menu callback to continue authentication process after user login. This
 * callback is encountered when a user tries to login to an RP but does not yet
 * have a valid local session
 */
function openid_provider_continue() {
  module_load_include('inc', 'openid');
  
  if (isset($_SESSION['openid_provider']['request'])) {
    $request = $_SESSION['openid_provider']['request'];
    unset($_SESSION['openid_provider']['request']);
    return openid_provider_endpoint($request);
  }
  else {
    drupal_set_message(t('Session expired'));
    drupal_goto();
  }
}

/**
 * Page callback for processing openid_provider_form.
 */
function openid_provider_send() {
  return drupal_get_form('openid_provider_form');
}


/**
 * Page callback for OpenID Sites form where users can configure the
 * auto_release status of RP's they have logged into.
 *
 * @param object $account User account object for the user.
 */
function openid_provider_sites($account) {
  return drupal_get_form('openid_provider_sites_form', $account);
}

/**
 * Form builder function for openid_provider_sites
 */
function openid_provider_sites_form($form_state, $user = NULL) {
  if (!$user) {
    global $user;
  }
  
  module_load_include('inc', 'openid_provider');
  
  $rps = _openid_provider_rp_load($user->uid);
  if (count($rps)) {
    $form = array('#tree' => TRUE);
    foreach ($rps as $rp) {
      $form['rpid'][$rp->rpid] = array(
        '#type' => 'checkbox',
        '#title' => $rp->realm,
        '#default_value' => $rp->auto_release,
        '#description' => t('Last accessed @time', array('@time' => format_date($rp->last_time)))
      );
    }
  
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save')
    );
  }
  else {
    $form['none']['#value'] = t('No sites yet');
  }
  return $form;
}

/**
 * Form submit callback for openid_provider_sites.
 */
function openid_provider_sites_form_submit($form, &$form_state) {
  foreach ($form_state['values']['rpid'] as $key => $value) {
    db_query("UPDATE {openid_provider_relying_party} SET auto_release=%d WHERE rpid=%d", $value, $key);
  }
  drupal_set_message(t('Settings saved.'));
}
