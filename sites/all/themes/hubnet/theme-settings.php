<?php

/**
 * Implementation of hook_settings() for themes.
 */
function hubnet_settings($settings) {
  // Add js & css
  drupal_add_css('misc/farbtastic/farbtastic.css', 'module', 'all', FALSE);
  drupal_add_js('misc/farbtastic/farbtastic.js');
  drupal_add_js(drupal_get_path('theme', 'hubnet') .'/hubnet-settings.js');

  $form = array();

  $form['header_color'] = array(
    '#title' => t('Header color'),
    '#type' => 'textfield',
    '#size' => '7',
    '#maxlength' => '7',
    '#default_value' => !empty($settings['header_color']) ? $settings['header_color'] : '#888888',
    '#suffix' => '<div id="hubnet-colorpicker-0"></div>',
  );

  $form['header_color_dark'] = array(
    '#title' => t('Header color dark'),
    '#type' => 'textfield',
    '#size' => '7',
    '#maxlength' => '7',
    '#default_value' => !empty($settings['header_color_dark']) ? $settings['header_color_dark'] : '#888888',
    '#suffix' => '<div id="hubnet-colorpicker-1"></div>',
  );

  return $form;
}