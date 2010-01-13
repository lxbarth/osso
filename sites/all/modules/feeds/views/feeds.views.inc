<?php
// $Id: feeds.views.inc,v 1.1 2009/10/20 21:03:08 alexb Exp $

/**
 * @file
 * Views integration for Feeds tables.
 *
 * @see http://drupal.org/project/views
 */

/**
 * Implementation of hook_views_data().
 */
function feeds_views_data() {
  $data = array();

  $data['feeds_source']['table'] = array(
    'group' => 'Feeds source',
    'base' => array(
      'field' => array('feed_nid', 'id'),
      'title' => 'Feeds source',
      'help' => 'The source information for a feed, for example its URL',
    ),
  );
  $data['feeds_source']['feed_nid'] = array(
    'title' => 'Feed node id',
    'help' => 'Contains the node id of a feed node if the feed\'s configuration is attached to a content type, otherwise contains 0.',
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
      'allow empty' => TRUE,
      'help' => 'Filter on a Feeds Source\'s feed_nid field.',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
      'validate type' => 'nid',
      'help' => 'Argument on a Feeds Source\'s feed_nid field.',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'help' => 'Sort Feeds Source\'s feed_nid field.',
    ),
  );
  $data['feeds_source']['config'] = array(
    'title' => 'Source',
    'help' => 'The resource identifier for this feed. For instance a URL.',
    'field' => array(
      'handler' => 'feeds_views_handler_field_source',
      'click sortable' => FALSE,
    ),
  );
  // feed_nid joins to node nid.
  $data['feeds_source']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'feed_nid',
      'type' => 'LEFT',
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function feeds_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'feeds') .'/views',
    ),
    'handlers' => array(
      // field handlers
      'feeds_views_handler_field_source' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}
