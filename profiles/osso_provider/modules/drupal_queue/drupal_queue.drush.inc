<?php
// $Id: drupal_queue.drush.inc,v 1.1 2009/10/31 12:50:58 alexb Exp $

/**
 * @file
 * Drush integration for Drupal Queue API.
 */

/**
 * Implementation of hook_drush_help().
 */
function drupal_queue_drush_help($section) {
  switch ($section) {
    case 'drush:queue cron':
      return dt('Run Drupal queue workers. As opposed to "drush cron" that can only be run one at a time on a single site, "drush queue cron" can be invoked as many times as the server load allows. Otherwise, schedule like drush cron.');
  }
}

/**
 * Implementation of hook_queue_drush_command().
 */
function drupal_queue_drush_command() {
  $items = array();
  $items['queue cron'] = array(
    'callback' => 'drupal_queue_cron_run',
    'description' => 'Run Drupal queue workers.',
  );
  return $items;
}