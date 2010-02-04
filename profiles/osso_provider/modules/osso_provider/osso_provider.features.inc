<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function osso_provider_user_default_permissions() {
  module_load_include('inc', 'osso_provider', 'osso_provider.defaults');
  $args = func_get_args();
  return call_user_func_array('_osso_provider_user_default_permissions', $args);
}
