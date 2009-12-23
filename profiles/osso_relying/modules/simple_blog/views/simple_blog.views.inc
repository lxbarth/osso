<?php

/**
 * @file
 * Views integration.
 */

/**
 * Implementation of hook_views_data().
 */
function simple_blog_views_data_alter(&$data) {

  // link to view comment
  $data['comments']['view_comment']['field']['handler'] = 'simple_blog_handler_field_comment_link';
}

/**
 * Implementation of hook_views_handlers().
 */
function simple_blog_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'simple_blog') . '/views',
    ),
    'handlers' => array(
      // Modified comment link field.
      'simple_blog_handler_field_comment_link' => array(
        'parent' => 'views_handler_field_comment_link',
      ),
    ),
  );
}