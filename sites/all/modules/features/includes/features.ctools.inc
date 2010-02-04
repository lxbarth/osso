<?php
// $Id: features.ctools.inc,v 1.1.2.6 2009/09/07 19:10:01 yhahn Exp $

/**
 * This is a wild hack, but effective.
 * Dynamically declare functions under a ctools component's namespace if they are not already declared.
 */
foreach (_ctools_features_get_info() as $component => $info) {
  $code = '';
  if (!function_exists("{$component}_features_export")) {
    $code .= 'function '. $component .'_features_export($data, &$export, $module_name = "") { return ctools_component_features_export("'. $component .'", $data, $export, $module_name); }';
  }
  if (!function_exists("{$component}_features_export_options")) {
    $code .= 'function '. $component .'_features_export_options() { return ctools_component_features_export_options("'. $component .'"); }';
  }
  if (!function_exists("{$component}_features_export_render")) {
    $code .= 'function '. $component .'_features_export_render($module = "foo", $data) { return ctools_component_features_export_render("'. $component .'", $module, $data); }';
  }
  eval($code);
}

/**
 * Implementation of hook_features_api().
 */
function ctools_features_api() {
  $components = _ctools_features_get_info();
  $components['ctools'] = array(
    'default_hook' => 'ctools_plugin_api',
    'feature_source' => TRUE,
    'duplicates' => FEATURES_DUPLICATES_ALLOWED,
  );
  return $components;
}

/**
 * Implementation of hook_features_export().
 * Adds references to the ctools mothership hook, ctools_plugin_api().
 */
function ctools_features_export($data, &$export, $module_name = '') {
  // Add ctools dependency
  $export['dependencies']['ctools'] = 'ctools';

  // Add the actual ctools components which will
  // need to be accounted for in hook_ctools_plugin_api().
  foreach ($data as $component) {
    $export['features']['ctools'][$component] = $component;
  }

  return array();
}

/**
 * Implementation of hook_features_export_render().
 * Adds the ctools mothership hook, ctools_plugin_api().
 */
function ctools_features_export_render($module = 'foo', $data) {
  $info = _ctools_features_get_info();

  $code = array();
  $code[] = '  $args = func_get_args();';
  $code[] = '  $module = array_shift($args);';
  $code[] = '  $api = array_shift($args);';

  $first = TRUE;
  foreach ($data as $component) {
    $module = $info[$component]['module'];
    $api = $info[$component]['api'];
    $version = $info[$component]['current_version'];

    $if = $first ? 'if' : 'else if';
    $code[] = '  '. $if .' ($module == "'. $module .'" && $api == "'. $api .'") {';
    $code[] = '    return array("version" => '. $version .');';
    $code[] = '  }';

    $first = FALSE;
  }
  return array('ctools_plugin_api' => implode("\n", $code));
}

/**
 * Master implementation of hook_features_export_options() for all ctools components.
 */
function ctools_component_features_export_options($component) {
  $options = array();

  ctools_include('export');
  $schema = ctools_export_get_schema($component);
  if ($schema) {
    $export_key = $schema['export']['key'];
    if ($objects = ctools_export_load_object($component, 'all')) {
      foreach ($objects as $object) {
        $key = $object->{$export_key};
        $options[$key] = $key;
      }
    }
  }
  return $options;
}

/**
 * Master implementation of hook_features_export() for all ctools components.
 */
function ctools_component_features_export($component, $data, &$export, $module_name = '') {
  // Add the actual implementing module as a dependency
  $info = _ctools_features_get_info();
  $export['dependencies'][$module] = $info[$component]['module'];

  // Add the components
  foreach ($data as $object_name) {
    $export['features'][$component][$object_name] = $object_name;
  }

  // Let CTools handle API integration for this component.
  return array('ctools' => array($component));
}

/**
 * Master implementation of hook_features_export_render() for all ctools components.
 */
function ctools_component_features_export_render($component, $module = 'foo', $data) {
  ctools_include('export');

  $schema = ctools_export_get_schema($component);
  $objects = ctools_export_load_object($component);
  $code = '  $export = array();'."\n";
  foreach ($data as $object) {
    $identifier = $schema['export']['identifier'];
    $code .= ctools_export_object($component, $objects[$object], '  ', $identifier);
    $code .= "\n";
    $code .= '  $export[\''. $object .'\'] = $'. $identifier .';'."\n";
  }
  $code .= '  return $export;';

  return array($schema['export']['default hook'] => $code);
}

/**
 * Helper function to return various ctools information for components.
 */
function _ctools_features_get_info($reset = FALSE) {
  static $components;
  if (!isset($components) || $reset) {
    $components = array();
    ctools_include('export');
    foreach (ctools_export_get_schemas_by_module() as $module => $schemas) {
      foreach ($schemas as $table => $schema) {
        $components[$table] = array(
          'api' => $schema['export']['api']['api'],
          'default_hook' => $schema['export']['default hook'],
          'current_version' => $schema['export']['api']['current_version'],
          'module' => $module,
          'feature_source' => TRUE,
        );
      }
    }
  }
  return $components;
}
