<?php
// $Id: node_form_input_format.inc,v 1.1 2009/04/18 02:00:35 merlinofchaos Exp $

/**
 * Implementation of specially named hook_ctools_content_types()
 */
function ctools_node_form_input_format_ctools_content_types() {
  return array(
    'single' => TRUE,
    'icon' => 'icon_node_form.png',
    'title' => t('Node form input format'),
    'description' => t('Input format for the body field on a node.'),
    'required context' => new ctools_context_required(t('Form'), 'node_form'),
    'category' => t('Form'),
  );
}

function ctools_node_form_input_format_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('node_form');

  $block->title = t('Input format');
  $block->delta = 'format-options';

  if (isset($context->form)) {
    if (!empty($context->form->form_id) && !empty($context->form['body_filter']['format'])) {
      // remove the fieldset
      unset($context->form['body_filter']['format']['#type']);
      $block->content = drupal_render($context->form['body_filter']['format']);
    }
  }
  else {
    $block->content = t('Input format.');
  }
  return $block;
}

function ctools_node_form_input_format_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form input format', array('@s' => $context->identifier));
}

function ctools_node_form_input_format_content_type_edit_form(&$form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
}
