<?php
// $Id: dependent.inc,v 1.5.2.1 2009/10/02 21:57:57 merlinofchaos Exp $

/**
 * @file
 * Provide dependent checkboxes that can be easily used in forms.
 *
 * This system will ensure that form items are invisible if the dependency is
 * not met. What this means is that you set the #dependency of an item to a
 * list of form ids that must be set, and the list of values that qualify.
 *
 * For a simple use, setting an item to be dependent upon a select box, if
 * any of the listed values are selected, the item will be visible. Otherwise,
 * the item will be visible.
 *
 * If dependent upon multiple items, use #dependency_count = X to set the
 * number of items that must be set in order to make this item visible. This
 * defaults to 1. If set to 2, then at least 2 form items in the list must
 * have their items set for the item to become visible.
 *
 * Checkboxes don't have their own id, so you need to add one in a div
 * around the checkboxes via #prefix and #suffix. You actually need to add TWO
 * divs because it's the parent that gets hidden. Also be sure to retain the
 * 'expand_checkboxes' in the #process array, because the views process will
 * override it.
 *
 * For radios, because they are selected a little bit differently, instead of
 * using the CSS id, use: radio:NAME where NAME is the #name of the property.
 * This can be quickly found by looking at the HTML of the generated form, but
 * it is usually derived from the array which contains the item. For example,
 * $form['menu']['type'] would have a name of menu[type]. This name is the same
 * field that is used to determine where in $form_state['values'] you will find
 * the value of the form.
 *
 * The item that is dependent on, should be set to #tree = TRUE.
 *
 * Usage:
 *
 * First, ensure this tool is loaded:
 * @code { ctools_include('dependent'); }
 *
 * On any form item, add
 * - @code '#process' => array('ctools_dependent_process'), @endcode
 * - @code '#dependency' => array('id-of-form-without-the-#' => array(list, of, values, that, make, this, gadget, visible)), @endcode
 *
 * A fuller example, that hides the menu title when no menu is selected:
 * @code
 *function ctools_dependent_example() {
 *  $form = array();
 *  $form['menu'] = array(
 *    '#type' => 'fieldset',
 *    '#title' => t('Menu settings'),
 *    '#tree' => TRUE,
 *  );
 *  $form['menu']['type'] = array(
 *    '#title' => t('Menu type'),
 *    '#type' => 'radios',
 *    '#options' => array(
 *      'none' => t('No menu entry'),
 *      'normal' => t('Normal menu entry'),
 *      'tab' => t('Menu tab'),
 *      'default tab' => t('Default menu tab'),
 *    ),
 *    '#default_value' => 'none',
 *  );
 *
 *  $form['menu']['title'] = array(
 *    '#title' => t('Title'),
 *    '#type' => 'textfield',
 *    '#default_value' => '',
 *    '#description' => t('If set to normal or tab, enter the text to use for the menu item.'),
 *    '#process' => array('ctools_dependent_process'),
 *    '#dependency' => array('radio:menu[type]' => array('normal', 'tab', 'default tab')),
 *   );
 *
 *  $form['menu']['title']['#process'] = array('ctools_dependent_process');
 *  $form['menu']['title']['#dependency'] =  array('radio:menu[type]' => array('normal', 'tab', 'default tab'));
 *
 *  return system_settings_form($form);
 *}
 * @endcode
 */

/**
 * Process callback to add dependency to form items.
 */
function ctools_dependent_process($element, $edit, &$form_state, &$form) {
  if (isset($element['#dependency'])) {
    if (!isset($element['#dependency_count'])) {
      $element['#dependency_count'] = 1;
    }
    if (!isset($element['#dependency_type'])) {
      $element['#dependency_type'] = 'hide';
    }

    $js = array(
      'values' => $element['#dependency'],
      'num' => $element['#dependency_count'],
      'type' => $element['#dependency_type'],
    );

    if (!empty($form_state['ajax'])) {
      $form_state['js settings']['CTools']['dependent'][$element['#id']] = $js;
    }
    else {
      ctools_add_js('dependent');
      $options['CTools']['dependent'][$element['#id']] = $js;
      drupal_add_js($options, 'setting');
    }
  }

  return $element;
}

