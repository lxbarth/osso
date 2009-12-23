<?php
// $Id: FeedsPlugin.inc,v 1.1 2009/10/20 21:03:08 alexb Exp $

/**
 * @file
 * Definition of FeedsPlugin class.
 */

/**
 * Implement source interface for all plugins.
 *
 * Note how this class does not attempt to store source information locally.
 * Doing this would break the model where source information is represented by
 * an object that is being passed into a Feed object and its plugins.
 *
 * There is only one instance of a Feed or a FeedsPlugin per feed configuration,
 * whereas there is one instance of a FeedsSource per feed configuration, per
 * source info definition.
 */
abstract class FeedsPlugin extends FeedsConfigurable implements FeedsSourceInterface {

  /**
   * Constructor.
   *
   * Initialize class variables.
   */
  protected function __construct($id) {
    parent::__construct($id);
    $this->source_config = $this->sourceDefaults();
  }

  /**
   * Save changes to the configuration of this object.
   * Delegate saving to parent (= Feed) which will collect
   * information from this object by way of getConfig() and store it.
   */
  public function save() {
    feeds_importer($this->id)->save();
  }

  /**
   * Returns TRUE if $this->sourceForm() returns a form.
   */
  public function hasSourceConfig() {
    $form = $this->sourceForm(array());
    return !empty($form);
  }

  /**
   * Implementation of FeedsSourceInterface::sourceDefaults().
   */
  public function sourceDefaults() {
    $values = array_flip(array_keys($this->sourceForm(array())));
    foreach ($values as $k => $v) {
      $values[$k] = '';
    }
    return $values;
  }

  /**
   * Callback methods, exposes source form.
   */
  public function sourceForm($source_config) {
    return array();
  }

  /**
   * Validation handler for sourceForm.
   */
  public function sourceFormValidate(&$values) {}
}

/**
 * Used when a plugin is missing.
 */
class FeedsMissingPlugin extends FeedsPlugin {
}