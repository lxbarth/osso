<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function push_user_ctools_plugin_api() {
  module_load_include('inc', 'push_user', 'push_user.defaults');
  $args = func_get_args();
  return call_user_func_array('_push_user_ctools_plugin_api', $args);
}

/**
 * Implementation of hook_keyauth_key_default().
 */
function push_user_keyauth_key_default() {
  module_load_include('inc', 'push_user', 'push_user.defaults');
  $args = func_get_args();
  return call_user_func_array('_push_user_keyauth_key_default', $args);
}
