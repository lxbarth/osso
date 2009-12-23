<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function simple_blog_user_default_permissions() {
  module_load_include('inc', 'simple_blog', 'simple_blog.defaults');
  $args = func_get_args();
  return call_user_func_array('_simple_blog_user_default_permissions', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function simple_blog_views_default_views() {
  module_load_include('inc', 'simple_blog', 'simple_blog.features.views');
  $args = func_get_args();
  return call_user_func_array('_simple_blog_views_default_views', $args);
}
