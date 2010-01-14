<?php
// $Id: simplecontext.inc,v 1.1 2009/08/17 22:02:26 merlinofchaos Exp $


/**
 * @file
 * Sample ctools context type plugin that shows how to create a context from an arg.
 *
 */

/**
 * Return a description of the plugin.
 */
function ctools_plugin_example_simplecontext_ctools_contexts() {
  return array(
    'title' => t("Simplecontext"),
    'description' => t('A single "simplecontext" context, or data element.'),
    'context' => 'ctools_plugin_example_context_create_simplecontext',  // func to create context
    'context name' => 'simplecontext',
    'settings form' => 'simplecontext_settings_form',
    'keyword' => 'simplecontext',
    'context name' => 'simplecontext',
    'placeholder form' => array(
      '#type' => 'textfield',
      '#description' => t('Enter some data to represent this "simplecontext".'),
    ),
  );
}

/**
 * Create a context, either from manual configuration or from an argument on the URL.
 *
 * @param $empty
 *   If true, just return an empty context.
 * @param $data
 *   If from settings form, an array as from a form. If from argument, a string.
 * @param $conf
 *   TRUE if the $data is coming from admin configuration, FALSE if it's from a URL arg.
 *
 * @return
 *   a Context object/
 */
function ctools_plugin_example_context_create_simplecontext($empty, $data = NULL, $conf = FALSE) {
  $context = new ctools_context('simplecontext');
  $context->plugin = 'simplecontext';

  if ($empty) {
    return $context;
  }

  if ($conf) {
    if (!empty($data)) {
      $context->data = new stdClass();
      // For this simple item we'll just create our data by stripping non-alpha and
      // adding '_from_configuration_item_1' to it.
      $context->data->description = preg_replace('/[^a-z]/i', '', $data['sample_simplecontext_setting']);
      $context->data->description .= '_from_configuration_sample_simplecontext_setting';
      $context->title = t("Simplecontext context from config");
      return $context;
    }
  }
  else {
    // $data is coming from an arg - it's just a string.
    // This is used for keyword.
    $context->title = $data;
    $context->argument = $data;
    // Make up a bogus context
    $context->data = new stdClass();
    // For this simple item we'll just create our data by stripping non-alpha and
    // adding '_from_simplecontext_argument' to it.
    $context->data->description = preg_replace('/[^a-z]/i', '', $data);
    $context->data->description .= '_from_simplecontext_argument';
    $context->arg_length = strlen($context->argument);
    return $context;
  }
}

function simplecontext_settings_form($conf, $external = FALSE) {
  if (empty($conf)) {
    $conf = array(
      'sample_simplecontext_setting' => 'default simplecontext setting',
    );
  }
  $form = array();
  $form['sample_simplecontext_setting'] = array(
    '#type' => 'textfield',
    '#title' => t('Setting for simplecontext'),
    '#size' => 50,
    '#description' => t('An example setting that could be used to configure a context'),
    '#default_value' => $conf['sample_simplecontext_setting'],
    '#prefix' => '<div class="clear-block no-float">',
    '#suffix' => '</div>',
  );
  return $form;
}

