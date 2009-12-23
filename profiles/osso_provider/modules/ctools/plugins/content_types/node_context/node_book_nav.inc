<?php
// $Id: node_book_nav.inc,v 1.1 2009/04/18 02:00:34 merlinofchaos Exp $

/**
 * Implementation of specially named hook_ctools_content_types()
 */
function ctools_node_book_nav_ctools_content_types() {
  if (module_exists('book')) {
    return array(
      'single' => TRUE,
      'title' => t('Book navigation'),
      'icon' => 'icon_node.png',
      'description' => t('The navigation menu the book the node belongs to.'),
      'required context' => new ctools_context_required(t('Node'), 'node'),
      'category' => t('Node'),
    );
  }
}

function ctools_node_book_nav_content_type_render($subtype, $conf, $panel_args, $context) {
  $node = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block = new stdClass();
  $block->module = 'book_nav';

  $block->title = t('Book navigation');
  if ($node) {
    $block->content = isset($node->book) ? theme('book_navigation', $node->book) : '';
    $block->delta = $node->nid;
  }
  else {
    $block->content = t('Book navigation goes here.');
    $block->delta = 'unknown';
  }

  return $block;
}

function ctools_node_book_nav_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" book navigation', array('@s' => $context->identifier));
}

function ctools_node_book_nav_content_type_edit_form(&$form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
}
