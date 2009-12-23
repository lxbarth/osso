<?php

/**
 * Helper to implementation of hook_user_default_permissions().
 */
function _simple_blog_user_default_permissions() {
  $permissions = array();

  // Exported permission: access comments
  $permissions[] = array(
    'name' => 'access comments',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: access user profiles
  $permissions[] = array(
    'name' => 'access user profiles',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: create story content
  $permissions[] = array(
    'name' => 'create story content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: delete own story content
  $permissions[] = array(
    'name' => 'delete own story content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: edit own story content
  $permissions[] = array(
    'name' => 'edit own story content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  return $permissions;
}
