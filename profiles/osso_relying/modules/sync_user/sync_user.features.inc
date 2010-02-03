<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function sync_user_ctools_plugin_api() {
  module_load_include('inc', 'sync_user', 'sync_user.defaults');
  $args = func_get_args();
  return call_user_func_array('_sync_user_ctools_plugin_api', $args);
}

/**
 * Implementation of hook_feeds_importer_default().
 */
function sync_user_feeds_importer_default() {
  module_load_include('inc', 'sync_user', 'sync_user.defaults');
  $args = func_get_args();
  return call_user_func_array('_sync_user_feeds_importer_default', $args);
}

/**
 * Implementation of hook_keyauth_key_default().
 */
function sync_user_keyauth_key_default() {
  module_load_include('inc', 'sync_user', 'sync_user.defaults');
  $args = func_get_args();
  return call_user_func_array('_sync_user_keyauth_key_default', $args);
}
