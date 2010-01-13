<?php
// $Id: FeedsSource.inc,v 1.1 2009/10/20 21:01:35 alexb Exp $

/**
 * @file
 * Definition of FeedsSourceInterface and FeedsSource class.
 */

/**
 * Declares an interface for a class that defines default values and form
 * descriptions for a FeedSource.
 */
interface FeedsSourceInterface {

  /**
   * Crutch: for ease of use, we implement FeedsSourceInterface for every
   * plugin, but then we need to have a handle which plugin actually implements
   * a source.
   *
   * @see FeedsPlugin class.
   *
   * @return
   *   TRUE if a plugin handles source specific configuration, FALSE otherwise.
   */
  public function hasSourceConfig();

  /**
   * Return an associative array of default values.
   */
  public function sourceDefaults();

  /**
   * Return a Form API form array that defines a form configuring values. Keys
   * correspond to the keys of the return value of sourceDefaults().
   */
  public function sourceForm($source_config);

  /**
   * Validate user entered values submitted by sourceForm().
   */
  public function sourceFormValidate(&$values);
}

/**
 * This exception gets thrown when no source is available
 * for a given feed_nid.
 *
 * @todo: don't use internal source loading + exception, but load source
 * object and pass it into FeedsImporter object.
 */
class FeedsNoSourceException extends Exception {}

/**
 * This class encapsulates a source of a feed. While a FeedsImporter object
 * contains a feed import configuration, a FeedsSource object is what passes
 * through such an import configuration, holds information about the feed's
 * source (e. g. the URL) and provides all the information for necessary for
 * fetching, parsing and processing of a feed source.
 *
 * FeedsPlugins that implement FeedsSourceInterface and return TRUE on
 * hasSourceConfig() can define default values and forms that allow for
 * configuration of the FeedsSource. For an example, see FeedsFetcher and
 * FeedsHTTPFetcher.
 *
 * It is important that a FeedsPlugin does not directly hold information about
 * a source but leave all storage up to FeedsSource. An instance of a
 * FeedsPlugin class only exists once per FeedsImporter configuration, while an
 * instance of a FeedsSource class exists once per feed_nid to be imported.
 *
 * As with Feed, the idea with FeedsSource is that it can be used without
 * actually saving the object to the database.
 */
class FeedsSource extends FeedsConfigurable {

  // Contains the node id of the feed this source info object is attached to.
  // Equals 0 if not attached to any node - i. e. if used on a
  // standalone import form within Feeds or by other API users.
  protected $feed_nid;

  // The FeedsImporter object that this source is expected to be used with.
  protected $importer;

  /**
   * Instantiate a unique object per class/id/feed_nid. Don't use
   * directly, use feeds_source() instead.
   */
  public static function instance($importer, $feed_nid = 0) {
    // This is useful at least as long as we're developing.
    if (empty($importer)) {
      throw new Exception(t('Empty FeedsImporter.'));
    }

    // Let others override which class to instantiate.
    $class = variable_get('feeds_source_class', 'FeedsSource');

    static $instances = array();
    if (!isset($instances[$class][$importer->id][$feed_nid])) {
      $instances[$class][$importer->id][$feed_nid] = new $class($importer, $feed_nid);
    }
    return $instances[$class][$importer->id][$feed_nid];
  }

  /**
   * Constructor.
   */
  protected function __construct(FeedsImporter $importer, $feed_nid) {
    $this->feed_nid = $feed_nid;
    $this->importer = $importer;
    parent::__construct($importer->id);
    $this->load();
  }

  /**
   * Save configuration.
   */
  public function save() {
    $config = $this->getConfig();
    // Store the source property of the fetcher in a separate column so that we
    // can do fast lookups on it.
    $source = '';
    if (isset($config[get_class($this->importer->fetcher)]['source'])) {
      $source = $config[get_class($this->importer->fetcher)]['source'];
    }
    $object = array(
      'id' => $this->id,
      'feed_nid' => $this->feed_nid,
      'config' => $config,
      'source' => $source,
    );
    // Make sure a source record is present at all time, try to update first,
    // then insert.
    drupal_write_record('feeds_source', $object, array('id', 'feed_nid'));
    if (!db_affected_rows()) {
      drupal_write_record('feeds_source', $object);
    }
  }

  /**
   * Load configuration and unpack.
   */
  public function load() {
    if ($config = db_result(db_query('SELECT config FROM {feeds_source} WHERE id = "%s" AND feed_nid = %d', $this->id, $this->feed_nid))) {
      // While FeedsSource cannot be exported, we still use CTool's export.inc
      // export definitions.
      // @todo: patch CTools to move constants from export.inc to ctools.module.
      ctools_include('export');
      $this->export_type = EXPORT_IN_DATABASE;
      $this->config = unserialize($config);
    }
  }

  /**
   * Delete configuration. Removes configuration information
   * from database, does not delete configuration itself.
   */
  public function delete() {
    db_query('DELETE FROM {feeds_source} WHERE id = "%s" AND feed_nid = %d', $this->id, $this->feed_nid);
  }

  /**
   * Convenience function. Returns the configuration for a specific class.
   *
   * @param FeedsSourceInterface $client
   *   An object that is an implementer of FeedsSourceInterface.
   *
   * @return
   *   An array stored for $client.
   */
  public function getConfigFor(FeedsSourceInterface $client) {
    return $this->config[get_class($client)];
  }

  /**
   * Return defaults for feed configuration.
   */
  public function configDefaults() {
    // Collect information from plugins.
    $defaults = array();
    foreach ($this->importer->plugin_types as $type) {
      if ($this->importer->$type->hasSourceConfig()) {
        $defaults[get_class($this->importer->$type)] = $this->importer->$type->sourceDefaults();
      }
    }
    return $defaults;
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    // Collect information from plugins.
    $form = array();
    foreach ($this->importer->plugin_types as $type) {
      if ($this->importer->$type->hasSourceConfig()) {
        $class = get_class($this->importer->$type);
        $form[$class] = $this->importer->$type->sourceForm($this->config[$class]);
        $form[$class]['#tree'] = TRUE;
      }
    }
    return $form;
  }

  /**
   * Override parent::configFormValidate().
   */
  public function configFormValidate(&$values) {
    foreach ($this->importer->plugin_types as $type) {
      $class = get_class($this->importer->$type);
      if (isset($values[$class]) && $this->importer->$type->hasSourceConfig()) {
        $this->importer->$type->sourceFormValidate($values[$class]);
      }
    }
  }
}
