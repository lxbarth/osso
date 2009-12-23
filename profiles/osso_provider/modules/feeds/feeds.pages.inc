<?php
// $Id: feeds.pages.inc,v 1.5 2009/10/21 23:18:30 alexb Exp $

/**
 * @file
 * Menu callbacks, form callbacks and helpers.
 */

/**
 * Render a page of available feed configuration.
 *
 * @todo: respect permissions.
 */
function feeds_page() {
  $rows = array();
  if ($importers = feeds_importer_load_all()) {
    foreach ($importers as $importer) {
      if ($importer->disabled) {
        continue;
      }
      if (empty($importer->config['content_type'])) {
        $link = 'import/'. $importer->id;
        $title = $importer->config['name'];
      }
      elseif (user_access('create '. $importer->config['content_type'] .' content')) {
        $link = 'node/add/'. str_replace('_', '-', $importer->config['content_type']);
        $title = node_get_types('name', $importer->config['content_type']);
      }
      $rows[] = array(
        l($title, $link),
        $importer->config['description'],
      );
    }
  }
  $header = array(
    t('Import'),
    t('Description'),
  );
  return theme('table', $header, $rows);
}

/**
 * Render a feeds import form on import/[config] pages.
 */
function feeds_import_form(&$form_state, $feed_id) {
  $importer = feeds_importer($feed_id);
  $source = feeds_source($importer, empty($form['nid']['#value']) ? 0 : $form['nid']['#value']);

  $form = array();
  $form['#importer_id'] = $importer->id;
  // @todo: Move this into fetcher?
  $form['#attributes']['enctype'] = 'multipart/form-data';
  $form['feeds'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );
  $form['feeds'] += $source->configForm($form_state);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

/**
 * Validation handler for node forms and feeds_import_form().
 *
 * Has to live in feeds.module, needs to be callable from node form.
 */
function feeds_import_form_validate($form, &$form_state) {
  $importer = feeds_importer($form['#importer_id']);
  $source = feeds_source($importer);
  // @todo: this may be a problem here, as we don't have a feed_nid at this point.
  $source->configFormValidate($form_state['values']['feeds']);
}

/**
 * Submit handler for feeds_import_form().
 */
function feeds_import_form_submit($form, &$form_state) {
  $importer = feeds_importer($form['#importer_id']);
  $source = feeds_source($importer);

  // Save source and import.
  $source->addConfig($form_state['values']['feeds']);
  $source->save();

  // Refresh feed if import on create is selected.
  if ($importer->config['import_on_create']) {
    $importer->import($source);
  }

  // Add importer to schedule.
  feeds_scheduler()->add($importer->id, 'import');
  feeds_scheduler()->add($importer->id, 'expire');
}

/**
 * Render a feeds import form on node/id/import pages.
 */
function feeds_import_tab_form(&$form_state, $node) {
  $importer = feeds_importer_by_content_type($node->type);

  $form = array();
  $form['#feed_nid'] = $node->nid;
  $form['#importer_id'] = $importer->id;
  return confirm_form($form, t('Import all content from feed?'), 'node/'. $node->nid, '', t('Import'), t('Cancel'), 'confirm feeds update');
}

/**
 * Submit handler for feeds_import_tab_form().
 */
function feeds_import_tab_form_submit($form, $form_state) {
  $importer = feeds_importer($form['#importer_id']);
  $source = feeds_source($importer, $form['#feed_nid']);
  $importer->import($source);
}

/**
 * Render a feeds delete form.
 *
 * Used on both node pages and configuration pages.
 * Therefore either $feed_id or $node may be missing.
 */
function feeds_delete_tab_form(&$form_state, $feed_id, $node = NULL) {
  if (empty($node)) {
    $importer = feeds_importer($feed_id);
    $path = 'import/'. $feed_id .'/delete-items';
  }
  else {
    $importer = feeds_importer_by_content_type($node->type);
    $form['#feed_nid'] = $node->nid;
    $path = 'node/'. $node->nid;
  }
  // Form cannot pass on feed object.
  $form['#importer_id'] = $importer->id;
  return confirm_form($form, t('Delete all items from feed?'), $path, '', t('Delete'), t('Cancel'), 'confirm feeds update');
}

/**
 * Submit handler for feeds_delete_tab_form().
 */
function feeds_delete_tab_form_submit($form, &$form_state) {
  $importer = feeds_importer($form['#importer_id']);
  $source = feeds_source($importer, empty($form['#feed_nid']) ? 0 : $form['#feed_nid']);
  $importer->clear($source);
}
