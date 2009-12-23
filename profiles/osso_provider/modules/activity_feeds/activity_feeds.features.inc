<?php

/**
 * Implementation of hook_content_default_fields().
 */
function activity_feeds_content_default_fields() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.defaults');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_content_default_fields', $args);
}

/**
 * Implementation of hook_context_default_contexts().
 */
function activity_feeds_context_default_contexts() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.defaults');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_context_default_contexts', $args);
}

/**
 * Implementation of hook_ctools_plugin_api().
 */
function activity_feeds_ctools_plugin_api() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.defaults');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_ctools_plugin_api', $args);
}

/**
 * Implementation of hook_data_default().
 */
function activity_feeds_data_default() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.defaults');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_data_default', $args);
}

/**
 * Implementation of hook_feeds_importer_default().
 */
function activity_feeds_feeds_importer_default() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.defaults');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_feeds_importer_default', $args);
}

/**
 * Implementation of hook_node_info().
 */
function activity_feeds_node_info() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.features.node');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_node_info', $args);
}

/**
 * Implementation of hook_user_default_permissions().
 */
function activity_feeds_user_default_permissions() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.defaults');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_user_default_permissions', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function activity_feeds_views_default_views() {
  module_load_include('inc', 'activity_feeds', 'activity_feeds.features.views');
  $args = func_get_args();
  return call_user_func_array('_activity_feeds_views_default_views', $args);
}
