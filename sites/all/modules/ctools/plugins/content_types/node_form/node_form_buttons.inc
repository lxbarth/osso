<?php
// $Id: node_form_buttons.inc,v 1.1 2009/04/18 02:00:35 merlinofchaos Exp $

/**
 * Implementation of specially named hook_ctools_content_types()
 */
function ctools_node_form_buttons_ctools_content_types() {
  return array(
    'single' => TRUE,
    'icon' => 'icon_node_form.png',
    'title' => t('Node form submit buttons'),
    'description' => t('Submit buttons for the node form.'),
    'required context' => new ctools_context_required(t('Form'), 'node_form'),
    'category' => t('Form'),
  );
}

function ctools_node_form_buttons_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('node_form');

  $block->title = '';
  $block->delta = 'buttons';

  if (isset($context->form)) {
    $block->content = drupal_render($context->form['buttons']);
  }
  else {
    $block->content = t('Node form buttons.');
  }
  return $block;
}

function ctools_node_form_buttons_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form submit buttons', array('@s' => $context->identifier));
}

function ctools_node_form_buttons_content_type_edit_form(&$form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
}
