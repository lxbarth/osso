<?php
// $Id: page_slogan.inc,v 1.1 2009/08/19 01:18:35 merlinofchaos Exp $

/**
 * @file
 * Plugin to handle the 'page_slogan' content type which allows the
 * slogan of the site to be embedded into a panel.
 */

/**
 * Implementation of hook_ctools_content_types()
 */
function ctools_page_slogan_ctools_content_types() {
  return array(
    'title' => t('Site Slogan'),
    'single' => TRUE,
    'icon' => 'icon_page.png',
    'description' => t('Add the slogan trail as content.'),
    'category' => t('Page elements'),
  );
}

/**
 * Output function for the 'page_slogan' content type.
 *
 * Outputs the slogan for the current page.
 */
function ctools_page_slogan_content_type_render($subtype, $conf, $panel_args) {
  $token = '<!-- ctools-page-slogan -->';
  ctools_set_page_token($token, 'variable', 'slogan');

  $block = new stdClass();
  $block->content = $token;

  return $block;
}
