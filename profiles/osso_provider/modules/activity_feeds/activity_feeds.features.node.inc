<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _activity_feeds_node_info() {
  $items = array(
    'activity_feed' => array(
      'name' => t('Activity feed'),
      'module' => 'features',
      'description' => t('Aggregates content and relates it to users.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '1',
      'body_label' => t('Body'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
