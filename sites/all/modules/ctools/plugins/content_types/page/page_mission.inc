<?php
// $Id: page_mission.inc,v 1.1 2009/08/19 01:18:35 merlinofchaos Exp $

/**
 * @file
 * Plugin to handle the 'page_mission' content type which allows the
 * mission statement of the site to be embedded into a panel.
 */

/**
 * Implementation of hook_ctools_content_types()
 */
function ctools_page_mission_ctools_content_types() {
  return array(
    'title' => t('Mission'),
    'single' => TRUE,
    'icon' => 'icon_page.png',
    'description' => t('Add the site mission statement as content.'),
    'category' => t('Page elements'),
  );
}

/**
 * Output function for the 'page_mission' content type.
 *
 * Outputs the mission statement for the site.
 */
function ctools_page_mission_content_type_render($subtype, $conf, $panel_args) {
  $token = '<!-- ctools-page-mission -->';
  ctools_set_page_token($token, 'variable', 'mission');

  $block = new stdClass();
  $block->content = $token;

  return $block;
}
