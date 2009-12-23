<?php
// $Id: views_rss.views.inc,v 1.1 2009/09/22 16:49:55 rsoden Exp $ 

/**
 * @file
 * Views plugins definition for views_kml
 */

/**
 * Implementation of hook_views_style_plugins(). Adds view types to views UI interface.
 */
function views_rss_views_plugins() {
  return array(
    'style' => array(
      'rss_fields' => array(
        'title' => t('RSS Feed - Fields'),
        'help' => t('Outputs a RSS formatted feed'),
        'handler' => 'views_plugin_style_rss_fields',
        'theme' => 'views_view_views_rss_fields',
        'theme file' => 'views_rss_views_fields.theme.inc',
        'theme path' => drupal_get_path('module', 'views_rss') .'/views',
        'path' => drupal_get_path('module', 'views_rss') .'/views',
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'uses grouping' => FALSE,
        'type' => 'feed',
      ),
    ),
  );
}
