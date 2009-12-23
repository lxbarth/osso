<?php
// $Id: node_form_publishing.inc,v 1.1 2009/04/18 02:00:35 merlinofchaos Exp $

/**
 * @file
 * Publishing options form for the node. This contains the basic settings
 * like published, moderated, node revision, etc.
 */

/**
 * Implementation of specially named hook_ctools_content_types()
 */
function ctools_node_form_publishing_ctools_content_types() {
  return array(
    'single' => TRUE,
    'title' => t('Node form publishing options'),
    'icon' => 'icon_node_form.png',
    'description' => t('Publishing options on the Node form.'),
    'required context' => new ctools_context_required(t('Form'), 'node_form'),
    'category' => t('Form'),
  );
}

function ctools_node_form_publishing_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();

  $block->title = t('Publishing options');
  $block->module = t('node_form');
  $block->delta = 'publishing-options';

  if (isset($context->form)) {
    if (!empty($context->form->form_id) && $context->form['options']['#type'] == 'fieldset') {
      // remove the fieldset
      unset($context->form['options']['#type']);
      $block->content = drupal_render($context->form['options']);
    }
  }
  else {
    $block->content = t('Publishing options.');
  }
  return $block;
}

function ctools_node_form_publishing_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form author information', array('@s' => $context->identifier));
}

function ctools_node_form_publishing_content_type_edit_form(&$form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
}
