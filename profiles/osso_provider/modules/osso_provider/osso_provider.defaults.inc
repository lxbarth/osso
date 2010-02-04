<?php

/**
 * Helper to implementation of hook_user_default_permissions().
 */
function _osso_provider_user_default_permissions() {
  $permissions = array();

  // Exported permission: change own username
  $permissions[] = array(
    'name' => 'change own username',
    'roles' => array(),
  );

  return $permissions;
}
