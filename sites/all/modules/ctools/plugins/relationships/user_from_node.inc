<?php
// $Id: user_from_node.inc,v 1.3.2.1 2009/10/01 22:00:09 merlinofchaos Exp $

/**
 * @file
 * Plugin to provide an relationship handler for node from user.
 */

/**
 * Implementation of specially named hook_ctools_relationships().
 */
function ctools_user_from_node_ctools_relationships() {
  return array(
    'title' => t('Node author'),
    'keyword' => 'user',
    'description' => t('Creates the author of a node as a user context.'),
    'required context' => new ctools_context_required(t('Node'), 'node'),
    'context' => 'ctools_user_from_node_context',
  );
  return $args;
}

/**
 * Return a new context based on an existing context.
 */
function ctools_user_from_node_context($context, $conf) {
  // If unset it wants a generic, unfilled context, which is just NULL.
  if (empty($context->data) || !isset($context->data->uid)) {
    return ctools_context_create_empty('user', NULL);
  }

  if (isset($context->data->uid)) {
    // Load the user that is the author of the node.
    $uid = $context->data->uid;
    $account = user_load(array('uid' => $uid));

    // Send it to ctools.
    return ctools_context_create('user', $account);
  }
}
