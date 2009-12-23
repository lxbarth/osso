<?php
// $Id: user_name.inc,v 1.1.2.1 2009/10/01 22:39:02 merlinofchaos Exp $

/**
 * @file
 *
 * Plugin to provide an argument handler for a username
 */

/**
 * Implementation of specially named hook_ctools_arguments().
 */
function ctools_user_name_ctools_arguments() {
  return array(
    'title' => t("User: name"),
    // keyword to use for %substitution
    'keyword' => 'user',
    'description' => t('Creates a user context from a user name.'),
    'context' => 'ctools_argument_user_name_context',
    'placeholder form' => array(
      '#type' => 'textfield',
      '#description' => t('Enter the username of a user for this argument'),
    ),
  );
}

/**
 * Discover if this argument gives us the user we crave.
 */
function ctools_argument_user_name_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('user');
  }

  // We can accept either a node object or a pure nid.
  if (is_object($arg)) {
    return ctools_context_create('user', $arg);
  }

  $account = user_load(array('name' => $arg));
  if (!$account) {
    return NULL;
  }
  return ctools_context_create('user', $account);
}



