<?php
// $Id: views_rss_views_fields.theme.inc,v 1.1 2009/09/22 16:49:55 rsoden Exp $ 

function template_preprocess_views_view_views_rss_fields(&$vars) {
  $view = $vars['view'];

  // Initialize namespaces array.
  $vars['ns'] = array();

  // Set basic info - title, description - about the feed
  $vars['viewtitle'] = variable_get('site_name', array()) . ' | ' . $view->display[$view->current_display]->display_title;
    
  if ($view->style_options['description']['feed_description']) {
    $description = $view->style_options['description']['feed_description'];
  }   
  else {
    $description = variable_get('site_mission', array());
  }
  $vars['description'] = theme_views_rss_feed_description($description, $view);
  
  // Base URL for link tag
  global $base_url;
  $vars['link'] = $base_url;
    
  // Grab the rows, push to field mapping function
  $items = $view->style_plugin->map_rows($vars['rows']);
  foreach($items as $item) {
    $rows .= theme('views_rss_fields_item', $item);
  }
  $vars['rows'] = $rows;

  // Render namespace definitions.
  $namespaces = array();
  if ($view->style_options['georss']['lat'] && $view->style_options['georss']['lon']) {
    $namespaces[] = 'xmlns:georss="http://www.georss.org/georss"';
  }
  if ($view->style_options['fields']['author_name']) {
    $namespaces[] = 'xmlns:dc="http://purl.org/dc/elements/1.1/"';
  }
  // error_log(print_r($view->style_options, true));
  $vars['namespaces'] = implode(' ', $namespaces);
}

/**
 * Preprocess for theme('views_rss_item').
 */
function template_preprocess_views_rss_fields_item(&$vars) { 
  $item = $vars['item'];
        
  // GeoRSS
  if ($item['lat'] && $item['lon']) {
    $item['georss:point'] = check_plain($item['lat'] . ' ' .  $item['lon']);
    unset($item['lat']);
    unset($item['lon']);
  }
  
  // dc:creator
  if ($item['author_name']) {
    $item['dc:creator'] = check_plain($item['author_name']);
    unset($item['author_name']);
  }
  
  // Loop through key=>value pairs
  foreach ($item as $key => $value) {
    if ($value) {
      $row .= '<' . $key . '>' . filter_xss_admin($value) . '</' . $key . '>
      ';
    }
  }

  $vars['row'] = $row;
}

/**
 * Theme function for feed icon.
 */
function theme_views_rss_feed_icon($url, $title, $icon) {
  if ($image = theme('image', $icon, t('Download RSS Feed'), $title)) {
    return '<a href="'. check_url($url) .'" class="feed-icon">'. $image .'</a>';
  }
}

/**
 * Theme function for feed description
 */
function theme_views_rss_feed_description($description, $view) {
  return $description;
}
