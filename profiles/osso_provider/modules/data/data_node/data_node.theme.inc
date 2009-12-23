<?php
// $Id: data_node.theme.inc,v 1.1.2.1 2009/09/22 17:40:45 alexb Exp $
/**
 * @file
 * Theme functions for Data Node module.
 */

/**
 * Theme a label for an existing item-node relationship.
 */
function theme_data_node_label($table, $id, $nid, $title) {
  drupal_add_css(drupal_get_path('module', 'data_node') . '/data_node.css');
  drupal_add_js(drupal_get_path('module', 'data_node') . '/data_node.js');

  $output = '';
  $output .= "<span class='data-node-label'>";
  $output .= l($title, "node/{$nid}", array('attributes' => array('class' => 'node-link'), 'query' => drupal_get_destination()));
  $output .= l('X', data_node_remove_path($table, $id, $nid), array('attributes' => array('class' => 'remove-link'), 'query' => drupal_get_destination()));
  $output .= "</span>";
  return $output;
}

/**
 * Theme for data_node_active_form.
 */
function theme_data_node_active_form($form) {
  drupal_add_css(drupal_get_path('module', 'data_node') . '/data_node.css');
  drupal_add_js(drupal_get_path('module', 'data_node') . '/data_node.js');

  $output = '';
  $output .= drupal_render($form['nid']);
  $output .= "<div class='new-node clear-block'>". drupal_render($form['new']) ."</div>";
  $output .= drupal_render($form);
  return $output;
}
