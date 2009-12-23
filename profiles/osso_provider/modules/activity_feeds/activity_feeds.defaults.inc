<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _activity_feeds_content_default_fields() {
  $fields = array();

  // Exported field: field_type
  $fields[] = array(
    'field_name' => 'field_type',
    'type_name' => 'activity_feed',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '1',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => 'Post
Comment',
    'allowed_values_php' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'value' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Activity type',
      'weight' => '35',
      'description' => 'The type of the activity.',
      'type' => 'optionwidgets_select',
      'module' => 'optionwidgets',
    ),
  );

  // Translatables
  array(
    t('Activity type'),
  );

  return $fields;
}

/**
 * Helper to implementation of hook_context_default_contexts().
 */
function _activity_feeds_context_default_contexts() {
  $items = array();

  $items[] = array(
    'namespace' => 'provider',
    'attribute' => 'section',
    'value' => 'frontpage',
    'description' => '',
    'path' => array(
      '<front>' => '<front>',
    ),
    'block' => array(
      'osso_provider_welcome_message' => array(
        'module' => 'osso_provider',
        'delta' => 'welcome_message',
        'weight' => 12,
        'region' => 'right',
        'status' => '0',
        'label' => 'Welcome message',
        'type' => 'context_ui',
      ),
    ),
  );
  $items[] = array(
    'namespace' => 'provider',
    'attribute' => 'section',
    'value' => 'login',
    'description' => '',
    'path' => array(
      'user' => 'user',
      'user/login' => 'user/login',
      'user/password' => 'user/password',
      'user/register' => 'user/register',
    ),
    'block' => array(
      'osso_provider_login_message' => array(
        'module' => 'osso_provider',
        'delta' => 'login_message',
        'weight' => 11,
        'region' => 'content',
        'status' => '0',
        'label' => 'Login page message',
        'type' => 'context_ui',
      ),
    ),
  );
  $items[] = array(
    'namespace' => 'provider',
    'attribute' => 'section',
    'value' => 'profile',
    'description' => 'User profile pages',
    'user' => array(
      '0' => 2,
    ),
    'block' => array(
      'osso_provider_dummy_profile' => array(
        'module' => 'osso_provider',
        'delta' => 'dummy_profile',
        'weight' => 11,
        'region' => 'right',
        'status' => '0',
        'label' => 'Dummy user profile',
        'type' => 'context_ui',
      ),
      'views_activity-block_2' => array(
        'module' => 'views',
        'delta' => 'activity-block_2',
        'weight' => 11,
        'region' => 'content',
        'status' => '0',
        'label' => 'activity: Current profile user',
        'type' => 'context_ui',
      ),
    ),
  );
  return $items;
}

/**
 * Helper to implementation of hook_ctools_plugin_api().
 */
function _activity_feeds_ctools_plugin_api() {
  $args = func_get_args();
  $module = array_shift($args);
  $api = array_shift($args);
  if ($module == "data" && $api == "data_default") {
    return array("version" => 1);
  }
  else if ($module == "feeds" && $api == "feeds_importer_default") {
    return array("version" => 1);
  }
}

/**
 * Helper to implementation of hook_data_default().
 */
function _activity_feeds_data_default() {
  $export = array();
  $data_table = new stdClass;
  $data_table->disabled = FALSE; /* Edit this to true to make a default data_table disabled initially */
  $data_table->api_version = 1;
  $data_table->title = 'Activity feed';
  $data_table->name = 'feeds_data_activity_feed';
  $data_table->table_schema = array(
    'fields' => array(
      'feed_nid' => array(
        'type' => 'int',
        'size' => 'normal',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'id' => array(
        'type' => 'serial',
        'size' => 'normal',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'timestamp' => array(
        'type' => 'int',
        'size' => 'normal',
        'unsigned' => TRUE,
        'not null' => FALSE,
      ),
      'title' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
      ),
      'description' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
      ),
      'uid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => FALSE,
      ),
      'url' => array(
        'type' => 'text',
        'not null' => FALSE,
      ),
    ),
    'name' => 'feeds_data_activity_feed',
    'indexes' => array(
      'feed_nid' => array(
        '0' => 'feed_nid',
      ),
      'id' => array(
        '0' => 'id',
      ),
      'timestamp' => array(
        '0' => 'timestamp',
      ),
      'url' => array(
        '0' => array(
          '0' => 'url',
          '1' => 255,
        ),
      ),
    ),
    'primary key' => array(
      '0' => 'id',
    ),
  );
  $data_table->meta = array(
    'fields' => array(
      'feed_nid' => array(
        'label' => '',
        'views_field_handler' => 'views_handler_field_numeric',
        'views_filter_handler' => 'views_handler_filter_numeric',
        'views_argument_handler' => 'views_handler_argument_numeric',
        'views_sort_handler' => 'views_handler_sort',
      ),
      'id' => array(
        'label' => '',
        'views_field_handler' => 'views_handler_field_numeric',
        'views_filter_handler' => 'views_handler_filter_numeric',
        'views_argument_handler' => 'views_handler_argument_numeric',
        'views_sort_handler' => 'views_handler_sort',
      ),
      'timestamp' => array(
        'label' => '',
        'views_field_handler' => 'views_handler_field_date',
        'views_filter_handler' => 'views_handler_filter_date',
        'views_argument_handler' => 'views_handler_argument_date',
        'views_sort_handler' => 'views_handler_sort_date',
      ),
      'title' => array(
        'label' => '',
        'views_field_handler' => 'views_handler_field',
        'views_filter_handler' => 'views_handler_filter_string',
        'views_argument_handler' => 'views_handler_argument_string',
        'views_sort_handler' => 'views_handler_sort',
      ),
      'description' => array(
        'label' => '',
        'views_field_handler' => 'views_handler_field',
        'views_filter_handler' => 'views_handler_filter_string',
        'views_argument_handler' => 'views_handler_argument_string',
        'views_sort_handler' => 'views_handler_sort',
      ),
      'uid' => array(
        'label' => '',
        'views_field_handler' => 'views_handler_field_numeric',
        'views_filter_handler' => 'views_handler_filter_numeric',
        'views_argument_handler' => 'views_handler_argument_numeric',
        'views_sort_handler' => 'views_handler_sort',
      ),
      'url' => array(
        'label' => '',
        'views_field_handler' => 'views_handler_field_url',
        'views_filter_handler' => 'views_handler_filter_string',
        'views_argument_handler' => 'views_handler_argument',
        'views_sort_handler' => 'views_handler_sort',
      ),
    ),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'feed_nid',
        'inner_join' => '1',
      ),
      'users' => array(
        'left_field' => 'uid',
        'field' => 'uid',
        'inner_join' => '1',
      ),
    ),
  );

  $export['feeds_data_activity_feed'] = $data_table;
  return $export;
}

/**
 * Helper to implementation of hook_feeds_importer_default().
 */
function _activity_feeds_feeds_importer_default() {
  $export = array();
  $feeds_importer = new stdClass;
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'activity_feed';
  $feeds_importer->config = array(
    'name' => 'Activity feed',
    'description' => 'Aggregates posts, comments and relates them to users.',
    'fetcher' => array(
      'plugin_key' => 'FeedsHTTPFetcher',
      'config' => array(
        'auto_detect_feeds' => FALSE,
      ),
    ),
    'parser' => array(
      'plugin_key' => 'ActivityFeedsUserParser',
      'config' => array(),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsDataProcessor',
      'config' => array(
        'update_existing' => 0,
        'expire' => -1,
        'mappings' => array(
          '0' => array(
            'source' => 'timestamp',
            'target' => 'timestamp',
            'unique' => 0,
          ),
          '1' => array(
            'source' => 'title',
            'target' => 'title',
            'unique' => 0,
          ),
          '2' => array(
            'source' => 'description',
            'target' => 'description',
            'unique' => 0,
          ),
          '3' => array(
            'source' => 'uid',
            'target' => 'uid',
            'unique' => 0,
          ),
          '4' => array(
            'source' => 'url',
            'target' => 'url',
            'unique' => 1,
          ),
        ),
      ),
    ),
    'content_type' => 'activity_feed',
    'update' => 0,
    'import_period' => '1800',
    'expire_period' => 3600,
    'import_on_create' => 1,
  );

  $export['activity_feed'] = $feeds_importer;
  return $export;
}

/**
 * Helper to implementation of hook_user_default_permissions().
 */
function _activity_feeds_user_default_permissions() {
  $permissions = array();

  // Exported permission: access user profiles
  $permissions[] = array(
    'name' => 'access user profiles',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  return $permissions;
}
