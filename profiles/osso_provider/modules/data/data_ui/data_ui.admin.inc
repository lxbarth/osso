<?php
// $Id: data_ui.admin.inc,v 1.25.2.14 2009/10/29 14:59:35 alexb Exp $
/**
 * @file
 * Admin UI functions.
 */

/**
 * List all tables for viewing content.
 */
function data_ui_view() {
  $tables = data_get_all_tables();
  $rows = array();
  foreach ($tables as $table) {
    $row = array(
      $table->get('title'),
      $table->get('name'),
      db_result(db_query('SELECT COUNT(*) FROM {'. db_escape_table($table->get('name')) .'}')),
    );
    if (module_exists('views')) {
      $row[] = l(t('View'), data_ui_get_default_path($table->get('name')));
    }
    $rows[] = $row;
  }
  $header = array(t('Title'), t('Name'), t('Number of rows'));
  if (module_exists('views')) {
    $header[] = '&nbsp;';
  }
  return theme('table', $header, $rows);
}

/**
 * Main page for data table management.
 */
function data_ui_manage() {
  $tables = data_get_all_tables();
  $rows = array();
  foreach ($tables as $table) {
    // Build operations depending on configuration status.
    $operations = array();
    if ($table->get('export_type') == EXPORT_IN_CODE) {
      $status = t('Default');
      $operations[] = l(t('Override'), 'admin/build/data/edit/'. $table->get('name'));
    }
    else if ($table->get('export_type') == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
      $status = t('Overridden');
      $operations[] = l(t('Edit'), 'admin/build/data/edit/'. $table->get('name'));
      $operations[] = l(t('Revert'), 'admin/build/data/revert/'. $table->get('name'));
    }
    else {
      $status = t('Normal');
      $operations[] = l(t('Edit'), 'admin/build/data/edit/'. $table->get('name'));
      $operations[] = l(t('Drop'), 'admin/build/data/drop/'. $table->get('name'));
    }
    if (module_exists('ctools')) {
      $operations[] = l(t('Export'), 'admin/build/data/export/'. $table->get('name'));
    }

    $row = array();
    $row[] = $table->get('title');
    $row[] = $table->get('name');
    $row[] = $status;
    $row[] = implode(' | ', $operations);

    $rows[] = $row;
  }
  $rows[] = array(
    l(t('Create new table'), 'admin/build/data/create'),
    '&nbsp;',
    '&nbsp;',
    '&nbsp;',
  );
  $header = array(t('Title'), t('Name'), t('Status'), t('Operations'));
  return theme('table', $header, $rows);
}

/**
 * Comparison page.
 */
function data_ui_compare() {
  $rows = array();
  $tables = data_get_all_tables();
  foreach ($tables as $table) {
    $row = array();
    $comp = $table->compareSchema();
    $row[] = $table->get('name');
    $status = $comp['status'];
    if ($status != 'same') {
      $status .= ' - '. l(t('adjust'), 'admin/build/data/compare/'. $table->get('name'));
    }
    $row[] = $status;
    $row[] = empty($comp['warning']) ? '-' : $comp['warning'];
    $rows[] = $row;
  }
  $header = array(t('Name'), t('Status'), t('Warnings'));
  return theme('table', $header, $rows);
}

/**
 * Adjust table schema form: Present the user with the difference between schema information and
 * the actual schema in the Database and offer three options:
 *
 * - Adjust schema info,
 * - adjust database or
 * - leave it.
 */
function data_ui_adjust_form(&$form_state, $table) {
  drupal_set_title(t('Adjust !table', array('!table' => $table->get('name'))));
  $comparison = $table->compareSchema();

  $form = array();
  $form['#redirect'] = 'admin/build/data/compare';
  $form['#table'] = $table;
  $form['#comparison'] = $comparison;
  $form['comparison'] = array(
    '#type' => 'fieldset',
    '#title' => t('Comparison'),
  );
  $form['comparison']['comparison']['#value'] = theme('data_ui_schema_compare_table', $comparison);

  if ($comparison['status'] == 'different') {
    $form['update_schema'] = array(
      '#type' => 'fieldset',
      '#title' => t('Option 1: Update schema information'),
      );
    $form['update_schema']['description'] = array(
      '#value' => t('<p>This option will update the schema information about this table.</p>'),
    );
    $form['update_schema']['submit'] = array(
      '#type' => 'submit',
      '#submit' => array('data_ui_adjust_form_submit_update_schema'),
      '#value' => t('Update schema information'),
    );
    $form['alter_table'] = array(
      '#type' => 'fieldset',
      '#title' => t('Option 2: Alter table'),
      );
    $form['alter_table']['description'] = array(
      '#value' => t('<p>Review the changes above carefully!
        This option will alter the database table and can very
        easily cause data loss.</p>'),
    );
    $form['alter_table']['submit'] = array(
      '#type' => 'submit',
      '#submit' => array('data_ui_adjust_form_submit_alter_table'),
      '#value' => t('Alter table'),
    );
  }
  elseif ($comparison['status'] == 'missing') {
    $form['alter_table'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create table'),
      );
    $form['alter_table']['description'] = array(
      '#value' => t('<p>Create a new table from schema information.</p>'),
    );
    $form['alter_table']['submit'] = array(
      '#type' => 'submit',
      '#submit' => array('data_ui_adjust_form_submit_create_table'),
      '#value' => t('Create table'),
    );
  }
  $form['cancel'] = array(
    '#type' => 'fieldset',
    '#title' => t('Don\'t change anything'),
    );
  $form['cancel']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );
  return $form;
}

/**
 * Submit handler for data_ui_adjust_form().
 */
function data_ui_adjust_form_submit_update_schema($form, &$form_state) {
  $table = $form['#table'];
  $schema = schema_invoke('inspect');
  if (isset($schema[$table->get('name')])) {
    $table->update(array('table_schema' => $schema[$table->get('name')]));
    drupal_set_message(t('Updated schema for !table', array('!table' => $table->get('name'))));
  }
  else {
    drupal_set_message(t('Error updating schema'), 'error');
  }
}

/**
 * Submit handler for data_ui_adjust_form().
 */
function data_ui_adjust_form_submit_alter_table($form, &$form_state) {
  $resolved = $resolved = array();
  if (isset($form['#comparison']['reasons'])) {
    foreach ($form['#comparison']['reasons'] as $field_reason) {
      if (_data_ui_alter_table($form['#table'], $field_reason)) {
        $resolved[] = $field_reason;
      }
      else {
        $unresolved[] = $field_reason;
      }
    }
  }
  if (count($resolved)) {
    drupal_set_message(t('Resolved') . theme('item_list', $resolved));
  }
  if (count($unresolved)) {
    drupal_set_message(t('Could not resolve') . theme('item_list', $unresolved), 'error');
  }
}

/**
 * Submit handler for data_ui_adjust_form().
 */
function data_ui_adjust_form_submit_create_table($form, &$form_state) {
  $table = $form['#table'];
  $ret = array();
  db_create_table($ret, $table->get('name'), $table->get('table_schema'));
  drupal_get_schema($table->get('name'), TRUE);
  if ($ret[0]['success']) {
    drupal_set_message(t('Created table !table', array('!table' => $table->get('name'))));
  }
  else {
    drupal_set_message(t('Error creating table'), 'error');
  }
}

/**
 * Form callback for adopt table form.
 *
 * 'Orphaned' tables are database tables that don't have an active schema
 * definition.
 */
function data_ui_adopt_form($form_state) {
  $form = array();

  // Compile a list of orphaned tables.
  $drupal_schema = drupal_get_schema(NULL, TRUE);
  $db_schema = schema_invoke('inspect');
  $orphaned_tables = array();
  foreach ($db_schema as $name => $table) {
    if (!isset($drupal_schema[$name])) {
      $orphaned_tables[$name] = $name;
    }
  }

  $form['orphaned_tables'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Orphaned Tables'),
    '#options' => $orphaned_tables,
  );
  if (count($orphaned_tables) < 1) {
    $form['no_orphaned_tables'] = array(
      '#type' => 'item',
      '#value' => t('There are no orphaned tables in your database.'),
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Adopt',
  );
  return $form;
}

/**
 * Submit handler for adopt table form.
 */
function data_ui_adopt_form_submit($form, &$form_state) {
  data_include('DataTable');
  $tables = array_keys(array_filter($form_state['values']['orphaned_tables']));
  foreach ($tables as $table_name) {
    $table = DataTable::instance($table_name);
    $table->adopt();
    unset($table);
  }

  DataTable::clearCaches();

  $form_state['redirect'] = 'admin/build/data';
}

/**
 * Form callback for create table form.
 */
function data_ui_create_form(&$form_state) {
  $form = array();
  if (!$form_state['storage']['field_num']) {
    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Table name'),
      '#description' => t('Machine readable name of the table - e. g. "my_table". Must only contain lower case letters and _.'),
      '#required' => TRUE,
    );
    $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Table title'),
      '#description' => t('Natural name of the table - e. g. "My Table".'),
      '#required' => TRUE,
    );
    $form['field_num'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of fields'),
      '#description' => t('The number of fields this table should contain.'),
      '#default_value' => 1,
      '#required' => TRUE,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
    );
  }
  else {
    $form['help']['#value'] = t('Define the fields of the new table.');
    $form['fields'] = array(
      '#tree' => TRUE,
    );
    for ($i = 0; $i < $form_state['storage']['field_num']; $i++) {
      $form['fields']['field_'. $i] = _data_ui_field_form(TRUE);
    }
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Create'),
    );
  }
  return $form;
}

/**
 * Validate handler for create table form.
 */
function data_ui_create_form_validate($form, &$form_state) {
  if (data_get_table(data_name($form_state['values']['name']))) {
    form_set_error('name', t('Name is already taken.'));
  }
  if (isset($form_state['values']['field_num'])) {
    if (is_numeric($form_state['values']['field_num'])) {
      if ($form_state['values']['field_num'] < 1) {
        form_set_error('field_num', t('At least one field must be created.'));
      }
    }
    else {
      form_set_error('field_num', t('Enter a number greater than 0.'));
    }
  }
  // Check for dupes.
  if (isset($form_state['values']['fields'])) {
    $names = array();
    foreach ($form_state['values']['fields'] as $field) {
      if (is_numeric($names[$field['name']])) {
        form_set_error('name', t('Names can\'t be numbers.'));
      }
      if (!isset($names[$field['name']])) {
        $names[$field['name']] = $field['name'];
      }
      else {
        form_set_error('name', t('Names must be unique.'));
      }
    }
  }
}

/**
 * Submit handler for create table form.
 */
function data_ui_create_form_submit($form, &$form_state) {

  if (isset($form_state['values']['field_num'])) {
    $form_state['storage'] = $form_state['values'];
  }
  elseif (isset($form_state['values']['fields'])) {

    // Create a schema from user input.
    $schema = $index = $primary = $meta = array();
    foreach ($form_state['values']['fields'] as $field) {
      $schema['fields'][$field['name']] = data_build_field_definition($field);
      $meta['fields'][$field['name']]['label'] = $field['label'];

      // Limit index if field type is text.
      if (!empty($field['index'])) {
        $index[$field['name']] = data_get_index_definition($field['name'], $field);
      }
      if (!empty($field['primary'])) {
        $primary[] = data_get_pk_definition($field['name'], $field);
      }
    }
    $schema['indexes'] = $index;
    $schema['primary key'] = $primary;

    // Create table.
    if ($table = data_create_table(data_name(trim($form_state['storage']['name'])), $schema, trim($form_state['storage']['title']))) {
      $meta = $table->update(array('meta' => $meta));
      drupal_set_message(t('Created table !table', array('!table' => $table->get('name'))));
    }
    else {
      drupal_set_message(t('Error creating table'), 'error');
    }

    // Unset storage to enable redirect.
    unset($form_state['storage']);
    $form_state['redirect'] = 'admin/build/data';
  }
}

/**
 * Form callback for revert table form.
 */
function data_ui_revert_form(&$form_state, $table) {
  $form = array();
  $form['#redirect'] = 'admin/build/data';
  $form['#table'] = $table;

  return confirm_form($form,
    t('Revert this table?'),
    'admin/build/data',
    t('Are you sure you would like to revert table !table? This will reset all information about this table its definition in code. This action cannot be undone.', array('!table' => $table->get('name'))),
    t('Revert'), t('Cancel')
  );
}

/**
 * Submit handler for data_ui_revert_form().
 */
function data_ui_revert_form_submit($form, &$form_state) {
  $table = $form['#table'];
  $table->revert();
}

/**
 * Form callback for drop table form.
 */
function data_ui_drop_form(&$form_state, $table) {
  $form = array();
  $form['#redirect'] = 'admin/build/data';
  $form['#table'] = $table;

  return confirm_form($form,
    t('Drop this table?'),
    'admin/build/data',
    t('Are you sure you would like to drop table !table? This action cannot be undone.', array('!table' => $table->get('name'))),
    t('Drop'), t('Cancel')
  );
}

/**
 * Submit handler for data_ui_drop_form().
 */
function data_ui_drop_form_submit($form, &$form_state) {
  $table = $form['#table'];
  data_drop_table($table->get('name'));
}

/**
 * Form callback for editing a table.
 */
function data_ui_edit_form(&$form_state, $table) {
  drupal_set_title($table->get('title'));

  $schema = $table->get('table_schema');
  $meta = $table->get('meta');

  $form = array();
  // Keep table.
  $form['table'] = array(
    '#type' => 'value',
    '#value' => $table,
  );

  // Existing fields.
  $form['fields'] = array('#tree' => TRUE);
  if (isset($schema['fields'])) {
    foreach ($schema['fields'] as $field_name => $field) {
      $form['fields'][$field_name] = array();
      $form['fields'][$field_name]['selected'] = array(
        '#type' => 'checkbox',
      );
      $form['fields'][$field_name]['name'] = array('#value' => $field_name);
      $form['fields'][$field_name]['label'] = array(
        '#type' => 'textfield',
        '#size' => 20,
        '#default_value' => $meta['fields'][$field_name]['label'],
      );
      $form['fields'][$field_name]['type'] = array(
        '#type' => 'select',
        '#options' => data_get_field_types(),
        '#default_value' => $field['type'],
      );
      $form['fields'][$field_name]['size'] = array(
        '#type' => 'select',
        '#options' => data_get_field_sizes(),
        '#default_value' => isset($field['size']) ? $field['size'] : 'normal',
      );
      $form['fields'][$field_name]['unsigned'] = array(
        '#type' => 'checkbox',
        '#default_value' => isset($field['unsigned']) ? $field['unsigned'] : FALSE,
      );
      $form['fields'][$field_name]['index'] = array(
        '#type' => 'checkbox',
        '#default_value' => isset($schema['indexes'][$field_name]),
      );
      $form['fields'][$field_name]['primary'] = array(
        '#type' => 'checkbox',
        '#default_value' => isset($schema['primary key']) ? in_array($field_name, $schema['primary key']) : FALSE,
      );
      if (isset($meta['join']) && $join = _data_ui_get_join($meta['join'], $field_name)) {
        $join = $join['left_table'] .'.'. $join['left_field'];
      }
      else {
        $join = t('<none>');
      }
      $join = l($join, 'admin/build/data/edit/'. $table->get('name') .'/join/'.$field_name);
      $form['fields'][$field_name]['join']['#value'] = $join;
    }
  }

  // Add a new field.
  $form['new'] = _data_ui_field_form();
  $form['new']['primary'] = array(
    '#type' => 'markup',
    '#value' => '&nbsp;',
    );
  $form['new']['join'] = array(
    '#type' => 'markup',
    '#value' => '&nbsp;',
    );
  $form['new']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add new'),
  );

  // Bulk operations.
  $options = array(
    t('Bulk operations'),
    'delete' => t('Delete all selected'),
  );
  $form['bulk_operation'] = array(
    '#type' => 'select',
    '#options' => $options,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit form.
 */
function data_ui_edit_form_submit($form, &$form_state) {
  $table = $form_state['values']['table'];
  $schema = $table->get('table_schema');

  if (!db_table_exists($table->get('name'))) {
    drupal_set_message(t('Table does not exist in database.'), 'error');
    if (module_exists('schema')) {
      drupal_set_message(t('Go to !compare to resolve conflicts.', array('!compare' => l(t('Compare schemas'), 'admin/build/data/compare'))), 'error');
    }
    return;
  }

  if ($form_state['clicked_button']['#value'] == t('Save')) {
    $fields = $schema['fields'];
    $new_fields = $form_state['values']['fields'];

    $new_index = array();
    $new_primary_key = array();

    if (empty($form_state['values']['bulk_operation']) && isset($fields)) {

      // Convert schema.
      foreach ($fields as $field_name => $field) {
        if ($new_spec = _data_ui_changed($new_fields[$field_name], $field)) {
          $table->changeField($field_name, $new_spec);
          drupal_set_message(t('Changed field !field_name', array('!field_name' => $field_name)));
        }
        if ($new_fields[$field_name]['index']) {
          $new_index[] = $field_name;
        }
        if ($new_fields[$field_name]['primary']) {
          $new_primary_key[] = $field_name;
        }
      }
      if (!$table->changeIndex($new_index)) {
        drupal_set_message(t('Error changing indexes'), 'error');
      }
      if (!$table->changePrimaryKey($new_primary_key)) {
        drupal_set_message(t('Error changing primary key'), 'error');

      }

      // Update meta data.
      $meta = $table->get('meta');
      foreach ($new_fields as $field_name => $field) {
        $meta['fields'][$field_name]['label'] = $field['label'];
      }
      $table->update(array('meta' => $meta));
      drupal_set_message(t('Saved changes'));
    }
    else {
      // Bulk updates.
      switch ($form_state['values']['bulk_operation']) {
        case 'delete':
          foreach ($new_fields as $field_name => $field) {
            if (!empty($field['selected'])) {
              // One field must stay.
              $schema = $table->get('table_schema');
              if (count($schema['fields']) > 1) {
                $table->dropField($field_name);
                drupal_set_message(t('Deleted field !field_name', array('!field_name' => $field_name)));
              }
              else {
                drupal_set_message('You cannot delete all fields from a table, drop the table instead.', 'error');
              }
            }
          }
          break;
      }
    }
  }
  elseif ($form_state['clicked_button']['#value'] == t('Add new')) {
    $new = $form_state['values']['new'];
    $spec = data_build_field_definition($new);
    if ($table->addField($new['name'], $spec)) {
      drupal_set_message(t('Added field !field', array('!field' => $new['name'])));
    }
    else {
      drupal_set_message(t('Error creating field'), 'error');
      return;
    }
    if (!empty($new['index'])) {
      if ($table->addIndex($new['name'])) {
        drupal_set_message(t('Added index for field !field', array('!field' => $new['name'])));
      }
      else {
        drupal_set_message(t('Error adding index'), 'error');
        return;
      }
    }
    $meta = $table->get('meta');
    $meta['fields'][$new['name']]['label'] = $new['label'];
    $table->update(array('meta' => $meta));
  }
}

/**
 * Edit title form.
 */
function data_ui_edit_title_form(&$form_state, $table) {
  drupal_set_title($table->get('title'));

  $form = array();
  $form['#table'] = $table;
  // Table title.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Natural name of the table.'),
    '#default_value' => $table->get('title'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit handler for data_ui_edit_title_form().
 */
function data_ui_edit_title_form_submit($form, &$form_state) {
  $form['#table']->update(array('title' => $form_state['values']['title']));
}

/**
 * Views handler configuration form.
 */
function data_ui_views_form(&$form_state, $table) {
  drupal_set_title($table->get('title'));

  module_load_include('inc', 'data', 'data.views');

  $schema = $table->get('table_schema');
  $meta = $table->get('meta');

  $form = array();
  // Keep table.
  $form['#table'] = $table;

  // Existing fields.
  $form['fields'] = array('#tree' => TRUE);
  if (isset($schema['fields'])) {
    foreach ($schema['fields'] as $field_name => $field) {
      $form['fields'][$field_name] = array();
      $form['fields'][$field_name]['name'] = array('#value' => $field_name);
      $form['fields'][$field_name]['views_field_handler'] = array(
        '#type' => 'select',
        '#options' => data_get_views_handler_options('field'),
        '#default_value' => data_get_views_handler('field', $table, $field_name),
      );
      $form['fields'][$field_name]['views_filter_handler'] = array(
        '#type' => 'select',
        '#options' => data_get_views_handler_options('filter'),
        '#default_value' => data_get_views_handler('filter', $table, $field_name),
      );
      $form['fields'][$field_name]['views_argument_handler'] = array(
        '#type' => 'select',
        '#options' => data_get_views_handler_options('argument'),
        '#default_value' => data_get_views_handler('argument', $table, $field_name),
      );
      $form['fields'][$field_name]['views_sort_handler'] = array(
        '#type' => 'select',
        '#options' => data_get_views_handler_options('sort'),
        '#default_value' => data_get_views_handler('sort', $table, $field_name),
      );
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for data_ui_views_form().
 */
function data_ui_views_form_submit($form, &$form_state) {
  $table = $form['#table'];
  $meta = $table->get('meta');
  if (isset($form_state['values']['fields'])) {
    foreach ($form_state['values']['fields'] as $field_name => $handlers) {
      foreach ($handlers as $handler_type => $handler) {
        $meta['fields'][$field_name][$handler_type] = $handler;
      }
    }
  }
  $table->update(array('meta' => $meta));
  views_invalidate_cache();
}

/**
 * Join form.
 */
function data_ui_join_form(&$form_state, $table, $field_name) {
  // drupal_set_title(t('Join field'));

  $schema = $table->get('table_schema');
  $meta = $table->get('meta');

  // Validate input.
  if (!isset($field_name) || !isset($schema['fields'][$field_name])) {
    drupal_set_message(t('Invalid field.'), 'error');
    drupal_goto('admin/build/data/edit/'. $table->get('name'));
  }

  // List all tables that schema API knows about as optoins.
  // @todo: This is a looong list - needs some AHAH to scale better.
  $table_schemas = drupal_get_schema();
  ksort($table_schemas);
  $options = array();
  foreach ($table_schemas as $table_name => $schema) {
    if ($table->get('name') != $table_name) {
      foreach ($schema['fields'] as $name => $info) {
        $options[$table_name][$table_name .'.'. $name] = $table_name .'.'. $name;
      }
    }
  }

  // Build form.
  $form = array();
  $form['#table'] = $table;
  $form['#field_name'] = $field_name;
  $form['#redirect'] = 'admin/build/data/edit/'. $table->get('name');
  $join = _data_ui_get_join($meta['join'], $field_name);
  $form['#original_join'] = $join;
  $form['left'] = array(
    '#type' => 'select',
    '#title' => t('Join !table_field to', array('!table_field' => $table->get('name') .'.'. $field_name)),
    '#options' => $options,
    '#default_value' => $join['left_table'] .'.'. $join['left_field'],
  );
  $form['inner_join'] = array(
    '#type' => 'radios',
    '#title' => t('Join type'),
    '#options' => array(t('Left join'), t('Inner join')),
    '#default_value' => $join['inner_join'] ? $join['inner_join'] : 0,
  );

  // Use confirm form for its formatting.
  $form = confirm_form($form,
    t('Join field'),
    'admin/build/data/edit/'. $table->get('name'),
    '',
    t('Save'), t('Cancel')
  );
  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array('data_ui_join_form_submit_delete'),
  );
  krsort($form['actions']);

  return $form;
}

/**
 * Submit handler for data_ui_join_form().
 */
function data_ui_join_form_submit($form, &$form_state) {
  list($left_table, $left_field) = explode('.', $form_state['values']['left']);
  $form['#table']->link($left_table, $left_field, $form['#field_name'], $form_state['values']['inner_join']);
  drupal_set_message(t('Updated join information.'));
}

/**
 * Submit handler for data_ui_join_form() - handles deletion.
 */
function data_ui_join_form_submit_delete($form, &$form_state) {
  // Use the original join information.
  $form['#table']->unlink($form['#original_join']['left_table']);
  drupal_set_message(t('Removed join information.'));
}

/**
 * Export form.
 */
function data_ui_export_form(&$form_state, $table) {
  $code = data_export($table->get('name'));

  $form['export'] = array(
    '#title' => t('Export table definition'),
    '#type' => 'textarea',
    '#value' => $code,
    '#rows' => substr_count($code, "\n"),
  );
  return $form;
}

/**
 * View table with krumo().
 */
function data_ui_view_schema($table) {
  drupal_set_title($table->get('title'));
  $output = '<H3>'. t('Schema') .'</H3>';
  $output .= kprint_r($table->get('table_schema'), TRUE);
  if ($meta = $table->get('meta')) {
    $output .= '<H3>'. t('Meta info') .'</H3>';
    $output .= kprint_r($table->get('meta'), TRUE);
  }
  return $output;
}

/**
 * Theme data_ui_create_form.
 */
function theme_data_ui_create_form($form) {

  // Render field definition form elements in a table.
  if (isset($form['fields'])) {
    $output = drupal_render($form['help']);

    $rows = array();
    foreach (element_children($form['fields']) as $e) {
      $row = array();
      foreach (element_children($form['fields'][$e]) as $f) {
        $row[] = drupal_render($form['fields'][$e][$f]);
      }
      $rows[] = $row;
    }
    $header = array(t('Name *'), t('Label'), t('Type'), t('Size'), t('Unsigned'), t('Index'), t('Primary key'));
    $output .= theme('table', $header, $rows);

    $output .= drupal_render($form);
    return $output;
  }
  return drupal_render($form);
}

/**
 * Theme data_ui_edit_form.
 */
function theme_data_ui_edit_form($form) {

  // Format existing fields.
  $rows = array();
  foreach (element_children($form['fields']) as $e) {
    $row = array();
    foreach (element_children($form['fields'][$e]) as $f) {
      $row[] = drupal_render($form['fields'][$e][$f]);
    }
    $row[] = '&nbsp;';
    $rows[] = $row;
  }

  // New fields form.
  $row = array('&nbsp;');
  foreach (element_children($form['new']) as $e) {
    $row[] = drupal_render($form['new'][$e]);
  }
  $rows[] = $row;

  $header = array(t('Select'), t('Name'), t('Label'), t('Type'), t('Size'), t('Unsigned'), t('Index'), t('Primary key'), t('Joins'));
  $output = theme('table', $header, $rows);
  $output .= drupal_render($form);
  return $output;
}

/**
 * Theme data_ui_views_form.
 */
function theme_data_ui_views_form($form) {

  // Format existing fields.
  $rows = array();
  foreach (element_children($form['fields']) as $e) {
    $row = array();
    foreach (element_children($form['fields'][$e]) as $f) {
      $row[] = drupal_render($form['fields'][$e][$f]);
    }
    $row[] = '&nbsp;';
    $rows[] = $row;
  }

  $header = array(t('Name'), t('Field handler'), t('Filter handler'), t('Argument handler'), t('Sort handler'));
  $output = theme('table', $header, $rows);
  $output .= drupal_render($form);
  return $output;
}

/**
 * Theme a schema module comparison result. Ie. the result of schema_compare_table().
 *
 * @todo: move to schema module - write a patch.
 */
function theme_data_ui_schema_compare_table($comparison) {
  $output = '';
  foreach ($comparison as $k => $v) {
    if (!empty($v)) {
      if (is_string($k)) {
        $output .= '<dl>';
        $output .= '<dt>'. ucfirst($k) .':</dt>';
        $output .= '<dd>';
        if (is_string($v)) {
          $output .= $v;
        }
        elseif (is_array($v)) {
          $output .= theme('item_list', $v);
        }
        $output .= '</dd>';
        $output .= '</dl>';
      }
    }
  }
  return $output;
}

/**
 * Magic helper function. Detect changed between keys in $new and $field
 * and return a new field spec based on $field IF there are differences.
 *
 * Otherwise return FALSE.
 *
 * Currently checked: type, unsigned
 */
function _data_ui_changed($new, $field) {
  $changed = FALSE;
  if ($field['type'] != $new['type']) {
    $field['type'] = $new['type'];
    $changed = TRUE;
  }
  if ($field['unsigned'] != $new['unsigned']) {
    $field['unsigned'] = $new['unsigned'];
    $changed = TRUE;
  }
  if ($changed) {
    return $field;
  }
  return FALSE;
}

/**
 * Helper function that generates a form snippet for defining a field.
 */
function _data_ui_field_form($required = FALSE) {
  $form = array();
  $form['#tree'] = TRUE;
  $form['name'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#required' => $required,
  );
  $form['label'] = array(
    '#type' => 'textfield',
    '#size' => 20,
  );
  $form['type'] = array(
    '#type' => 'select',
    '#options' => data_get_field_types(),
  );
  $form['size'] = array(
    '#type' => 'select',
    '#options' => data_get_field_sizes(),
  );
  $form['unsigned'] = array(
    '#type' => 'checkbox',
  );
  $form['index'] = array(
    '#type' => 'checkbox',
  );
  $form['primary'] = array(
    '#type' => 'checkbox',
  );
  return $form;
}

/**
 * Helper function to get link information for a specific field.
 */
function _data_ui_get_join($join, $field) {
  if (is_array($join)) {
    foreach ($join as $left_table => $info) {
      if ($info['field'] == $field) {
        $info['left_table'] = $left_table;
        return $info;
      }
    }
  }
  return FALSE;
}

/**
 * Helper function for adjusting a table's real schema.
 * @todo: this should live in schema module and should use better defined $reason keys.
 */
function _data_ui_alter_table($table, $field_reason) {
  list($field, $reason) = explode(': ', $field_reason);
  $schema = $table->get('table_schema');

  switch ($reason) {
    case 'not in database':
      if (isset($schema['fields'][$field])) {
        return $table->addField($field, $schema['fields'][$field]);
      }
      return FALSE;

    case 'missing in database':
      list($type, $field) = explode(' ', $field);
      // @todo: support multiple keys.
      if ($type == 'indexes') {
        return $table->addIndex($field);
      }
      elseif ($type == 'unique keys') {
        return $table->addUniqueKey($field);
      }
      elseif ($type == 'primary key') {
        return $table->addPrimaryKey($schema['primary keys']);
      }
      return FALSE;

    case 'primary key:<br />declared': // @todo: yikes!
      $table->dropPrimaryKey();
      return $table->changePrimaryKey($schema['primary keys']);
    case 'missing in schema':
      if ($field == 'primary key') {
        return $table->dropPrimaryKey();
      }
      return FALSE;

    case 'unexpected column in database':
      return $table->dropField($field);

  }
  return FALSE;
}
