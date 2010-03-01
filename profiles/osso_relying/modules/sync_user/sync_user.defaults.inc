<?php

/**
 * Helper to implementation of hook_ctools_plugin_api().
 */
function _sync_user_ctools_plugin_api() {
  $args = func_get_args();
  $module = array_shift($args);
  $api = array_shift($args);
  if ($module == "feeds" && $api == "feeds_importer_default") {
    return array("version" => 1);
  }
  else if ($module == "keyauth" && $api == "keyauth_key_default") {
    return array("version" => 1);
  }
}

/**
 * Helper to implementation of hook_feeds_importer_default().
 */
function _sync_user_feeds_importer_default() {
  $export = array();
  $feeds_importer = new stdClass;
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'user_updates';
  $feeds_importer->config = array(
    'name' => 'User updates',
    'description' => 'Synchronizes users on this site to users on OpenID Provider.',
    'fetcher' => array(
      'plugin_key' => 'KeyAuthHTTPFetcher',
      'config' => array(
        'use_pubsubhubbub' => 1,
        'designated_hub' => '',
        'use_keyauth' => 1,
        'keyauth_public' => '7fb5490cce31840608ec635a931c00aa',
      ),
    ),
    'parser' => array(
      'plugin_key' => 'SyncUserParser',
      'config' => array(),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsUserProcessor',
      'config' => array(
        'roles' => array(),
        'update_existing' => 1,
        'status' => '1',
        'mappings' => array(
          '0' => array(
            'source' => 'name',
            'target' => 'name',
            'unique' => 0,
          ),
          '1' => array(
            'source' => 'mail',
            'target' => 'mail',
            'unique' => 0,
          ),
          '2' => array(
            'source' => 'openid',
            'target' => 'openid',
            'unique' => 1,
          ),
        ),
      ),
    ),
    'content_type' => 'page',
    'update' => 0,
    'import_period' => '604800',
    'expire_period' => 3600,
    'import_on_create' => 0,
  );

  $export['user_updates'] = $feeds_importer;
  return $export;
}

/**
 * Helper to implementation of hook_keyauth_key_default().
 */
function _sync_user_keyauth_key_default() {
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
