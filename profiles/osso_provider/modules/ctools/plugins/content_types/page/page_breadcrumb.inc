<?php
// $Id: page_breadcrumb.inc,v 1.1 2009/08/17 21:23:13 merlinofchaos Exp $

/**
 * @file
 * Plugin to handle the 'page_breadcrumb' content type which allows the
 * breadcrumb trail of the current page to be embedded into a panel.
 */

/**
 * Implementation of hook_ctools_content_types()
 */
function ctools_page_breadcrumb_ctools_content_types() {
  return array(
    'title' => t('Breadcrumb'),
    'single' => TRUE,
    'icon' => 'icon_page.png',
    'description' => t('Add the breadcrumb trail as content.'),
    'category' => t('Page elements'),
  );
}

/**
 * Output function for the 'page_breadcrumb' content type.
 *
 * Outputs the breadcrumb for the current page.
 */
function ctools_page_breadcrumb_content_type_render($subtype, $conf, $panel_args) {
  $token = '<!-- ctools-page-breadcrumb -->';
  ctools_set_page_token($token, 'variable', 'breadcrumb');

  $block = new stdClass();
  $block->content = $token;

  return $block;
}

