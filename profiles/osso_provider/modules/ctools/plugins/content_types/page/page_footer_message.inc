<?php
// $Id: page_footer_message.inc,v 1.1 2009/09/21 20:53:00 merlinofchaos Exp $

/**
 * @file
 * Plugin to handle the 'page' content type which allows the standard page
 * template variables to be embedded into a panel.
 */

/**
 * Implementation of hook_ctools_content_types()
 */
function ctools_page_footer_message_ctools_content_types() {
  return array(
    'single' => TRUE,
    'title' => t('Page footer message'),
    'icon' => 'icon_page.png',
    'description' => t('Add the page footer message as content.'),
    'category' => t('Page elements'),
  );
}

/**
 * Output function for the 'page_footer_message' content type.
 *
 * Outputs the page footer message of the current page.
 */
function ctools_page_footer_message_content_type_render($subtype, $conf, $panel_args) {
  $token = '<!-- ctools-page-footer-message -->';
  ctools_set_page_token($token, 'variable', 'footer_message');

  $block = new stdClass();
  $block->content = $token;

  return $block;
}