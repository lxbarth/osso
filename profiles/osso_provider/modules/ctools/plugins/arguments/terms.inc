<?php
// $Id: terms.inc,v 1.3.2.2 2009/10/01 22:39:02 merlinofchaos Exp $

/**
 * @file
 *
 * Plugin to provide an argument handler for a Taxonomy term
 */

/**
 * Implementation of specially named hook_ctools_arguments().
 */
function ctools_terms_ctools_arguments() {
  return array(
    'title' => t("Taxonomy term (multiple): ID"),
    // keyword to use for %substitution
    'keyword' => 'term',
    'description' => t('Creates a group of taxonomy terms from a list of tids separated by a comma or a plus sign. In general the first term of the list will be used for panes.'),
    'context' => 'ctools_terms_context',
    'default' => array('breadcrumb' => TRUE),
    'settings form' => 'ctools_terms_settings_form',
    'placeholder form' => array(
      '#type' => 'textfield',
      '#description' => t('Enter a term ID or a list of term IDs separated by a + or a ,'),
    ),
    'breadcrumb' => 'ctools_terms_breadcrumb',
  );
}

/**
 * Discover if this argument gives us the term we crave.
 */
function ctools_terms_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('terms');
  }

  $terms = ctools_break_phrase($arg);
  if (empty($terms->value) || !empty($terms->invalid_input)) {
    return FALSE;
  }

  $context = ctools_context_create('terms', $terms);
  $context->original_argument = $arg;
  return $context;
}

/**
 * Settings form for the argument
 */
function ctools_terms_settings_form(&$form, &$form_state, $conf) {
  $form['settings']['breadcrumb'] = array(
    '#title' => t('Inject hierarchy of first term into breadcrumb trail'),
    '#type' => 'checkbox',
    '#default_value' => !empty($conf['breadcrumb']),
    '#description' => t('If checked, taxonomy term parents will appear in the breadcrumb trail.'),
  );
}

/**
 * Inject the breadcrumb trail if necessary.
 */
function ctools_terms_breadcrumb($conf, $context) {
  if (empty($conf['breadcrumb'])) {
    return;
  }

  $current->tid = $context->tids[0];
  $breadcrumb = array();
  while ($parents = taxonomy_get_parents($current->tid)) {
    $current = array_shift($parents);
    $breadcrumb[] = l($current->name, 'taxonomy/term/' . $current->tid);
  }

  $breadcrumb = array_merge(drupal_get_breadcrumb(), array_reverse($breadcrumb));
  drupal_set_breadcrumb($breadcrumb);
}
