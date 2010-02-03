<?php

/**
 * Helper to implementation of hook_ctools_plugin_api().
 */
function _push_user_ctools_plugin_api() {
  $args = func_get_args();
  $module = array_shift($args);
  $api = array_shift($args);
  if ($module == "keyauth" && $api == "keyauth_key_default") {
    return array("version" => 1);
  }
}

/**
 * Helper to implementation of hook_keyauth_key_default().
 */
function _push_user_keyauth_key_default() {
  $export = array();
  $keyauth_keys = new stdClass;
  $keyauth_keys->disabled = FALSE; /* Edit this to true to make a default keyauth_keys disabled initially */
  $keyauth_keys->api_version = 1;
  $keyauth_keys->title = 'OpenID Simple Sign-On';
  $keyauth_keys->public_key = '7fb5490cce31840608ec635a931c00aa';
  $keyauth_keys->private_key = '9315217ea879390698f2562476fd02de974998b7';

  $export['7fb5490cce31840608ec635a931c00aa'] = $keyauth_keys;
  return $export;
}
