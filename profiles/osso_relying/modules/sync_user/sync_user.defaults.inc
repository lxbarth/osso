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
      'plugin_key' => 'FeedsPubSubFetcher',
      'config' => array(
        'subscription_period' => 86400,
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
