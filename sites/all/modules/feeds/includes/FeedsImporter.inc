<?php
// $Id: FeedsImporter.inc,v 1.4 2009/10/21 22:49:47 alexb Exp $

/**
 * @file
 * FeedsImporter class and related.
 */

// Including FeedsImporter.inc automatically includes dependencies.
require_once(dirname(__FILE__) .'/FeedsConfigurable.inc');
require_once(dirname(__FILE__) .'/FeedsSource.inc');

/**
 * A Feeds result class.
 *
 * @see class FeedsFetcherResult
 * @see class FeedsParserResult
 */
abstract class FeedsResult {

  // An array of valid values for $type.
  protected $valid_types = array();
  // The type of this result.
  protected $type;
  // The value of this result.
  protected $value;

  /**
   * Constructor: create object, validate class variables.
   *
   * @param $value
   *   The value of this result.
   * @param $type
   *   The type of this result. Must be one of $valid_types.
   */
  public function __construct($value, $type) {
    $this->__set('type', $type);
    $this->__set('value', $value);
  }

  /**
   * Control access to class variables.
   */
  public function __set($name, $value) {
    if ($name == 'valid_types') {
      throw new Exception(t('Cannot write FeedsResult::valid_types.'));
    }
    if ($name == 'type') {
      if (!in_array($value, $this->valid_types)) {
        throw new Exception(t('Invalid type "!type"', array('!type' => $value)));
      }
    }
    $this->$name = $value;
  }

  /**
   * Control access to class variables.
   */
  public function __get($name) {
    return $this->$name;
  }
}

/**
 * Class defining an importer object. This is the main hub for Feeds module's
 * functionality.
 *
 * A FeedsImporter holds a pointer to a fetcher, a parser and a processor
 * plugin. It further contains the configuration for itself and each of the
 * three plugins.
 *
 * Its most important responsibilities are configuration management, importing
 * and purging.
 *
 * When a FeedsImporter is instantiated, it loads its configuration. Then it
 * instantiates one fetcher, one parser and one processor plugin depending on
 * the configuration information. After instantiating them, it sets them to
 * the configuration information it holds for them.
 *
 * @see __construct()
 *
 * When importing or purging, a FeedsSource object is passed into import() and
 * the fetcher, the parser and the processor are subsequently executed. It is
 * important to note that at no time a FeedsImporter object holds a pointer to a
 * FeedsSource object, while a FeedsSource object always holds a pointer to a
 * FeedsImporter object. The reason is that there is only one FeedsImporter
 * instance per configuration, while there is a FeedsSource object per source to
 * be imported. Sources can be tied to feed nodes, thus there can be potentially
 * many sources per feeds configuration.
 *
 * @see import()
 * @see clear()
 */
class FeedsImporter extends FeedsConfigurable {

  // Every feed has a fetcher, a parser and a processor.
  // These variable names match the possible return values of
  // feeds_plugin_type().
  protected $fetcher, $parser, $processor;

  // This array defines the variable names of the plugins above.
  protected $plugin_types = array('fetcher', 'parser', 'processor');

  /**
   * Instantiate class variables, initialize and configure
   * plugins.
   */
  protected function __construct($id) {
    parent::__construct($id);

    // Try to load information from database.
    $this->load();

    // Instantiate fetcher, parser and processor, set their configuration if
    // stored info is available.
    foreach ($this->plugin_types as $type) {
      $plugin = feeds_plugin_instance($this->config[$type]['plugin_key'], $this->id);

      if (isset($this->config[$type]['config'])) {
        $plugin->setConfig($this->config[$type]['config']);
      }
      $this->$type = $plugin;
    }
  }

  /**
   * Import a feed: execute, fetching, parsing and processing stage.
   *
   * @throws Exception
   *   If a problem with fetching, parsing or processing occured.
   *   @todo: Iron out and document potential Exceptions.
   *   @todo: Support batching.
   *   @todo: catch exceptions outside of import(), clear() and expire().
   */
  public function import(FeedsSource $source) {
    try {
      $result = $this->fetcher->fetch($source);
      $result = $this->parser->parse($result, $source);
      $this->processor->process($result, $source);
    }
    catch (Exception $e) {
      drupal_set_message($e->getMessage(), 'error');
    }
    module_invoke_all('feeds_after_import', $this, $source);
  }

  /**
   * Remove all items from a feed.
   */
  public function clear(FeedsSource $source) {
    try {
      $this->fetcher->clear($source);
      $this->parser->clear($source);
      $this->processor->clear($source);
    }
    catch (Exception $e) {
      drupal_set_message($e->getMessage(), 'error');
    }
  }

  /**
   * Remove items older than $time. If $time is not given, processor settings
   * will be used.
   */
  public function expire($time = NULL) {
    try {
      $this->processor->expire($time);
    }
    catch (Exception $e) {
      drupal_set_message($e->getMessage(), 'error');
    }
  }

  /**
   * Get the refresh period for import() or expire().
   */
  public function getSchedulePeriod($callback) {
    if ($callback == 'import') {
      return $this->config['import_period'];
    }
    if ($callback == 'expire') {
      // If a processor has expiry time set, run expiry every hour.
      if (FEEDS_EXPIRE_NEVER != $this->processor->expiryTime()) {
        return 3600;
      }
      return FEEDS_SCHEDULE_NEVER;
    }
  }

  /**
   * Save configuration.
   */
  public function save() {
    $save = new stdClass();
    $save->id = $this->id;
    $save->config = $this->getConfig();
    // Make sure a source record is present at all time, try to update first,
    // then insert.
    drupal_write_record('feeds_importer', $save, 'id');
    if (!db_affected_rows()) {
      drupal_write_record('feeds_importer', $save);
    }
    // Clear menu cache, changes to importer can change menu items.
    menu_rebuild();
  }

  /**
   * Load configuration and unpack.
   */
  public function load() {
    ctools_include('export');
    if ($config = ctools_export_load_object('feeds_importer', 'conditions', array('id' => $this->id))) {
      $config = array_shift($config);
      $this->export_type = $config->export_type;
      $this->disabled = $config->disabled;
      $this->config = $config->config;
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Delete configuration. Removes configuration information
   * from database, does not delete configuration itself.
   */
  public function delete() {
    db_query('DELETE FROM {feeds_importer} WHERE id = "%s"', $this->id);
  }

  /**
   * Set plugin.
   *
   * @param $plugin_key
   *   A fetcher, parser or processor plugin.
   *
   * @todo: error handling, handle setting to the same plugin.
   */
  public function setPlugin($plugin_key) {
    // $plugin_type can be either 'fetcher', 'parser' or 'processor'
    if ($plugin_type = feeds_plugin_type($plugin_key)) {
      if ($plugin = feeds_plugin_instance($plugin_key, $this->id)) {
        // Unset existing plugin, switch to new plugin.
        unset($this->$plugin_type);
        $this->$plugin_type = $plugin;
        // Set configuration information, blow away any previous information on
        // this spot.
        $this->config[$plugin_type] = array('plugin_key' => $plugin_key);
      }
    }
  }

  /**
   * Copy a FeedsImporter configuration into this importer.
   *
   * @param FeedsImporter $importer
   *   The feeds importer object to copy from.
   */
  public function copy(FeedsImporter $importer) {
    $this->setConfig($importer->config);

    // Instantiate new fetcher, parser and processor and initialize their
    // configurations.
    foreach ($this->plugin_types as $plugin_type) {
      $this->setPlugin($importer->config[$plugin_type]['plugin_key']);
      $this->$plugin_type->setConfig($importer->config[$plugin_type]['config']);
    }
  }

  /**
   * Get configuration of this feed.
   */
  public function getConfig() {
    foreach (array('fetcher', 'parser', 'processor') as $type) {
      $this->config[$type]['config'] = $this->$type->getConfig();
    }
    return $this->config;// Collect information from plugins.
  }

  /**
   * Return defaults for feed configuration.
   */
  public function configDefaults() {
    return array(
      'name' => '',
      'description' => '',
      'fetcher' => array(
        'plugin_key' => 'FeedsHTTPFetcher',
      ),
      'parser' => array(
        'plugin_key' => 'FeedsSyndicationParser',
      ),
      'processor' => array(
        'plugin_key' => 'FeedsNodeProcessor',
      ),
      'content_type' => 'page', // @todo: provide default content type feed.
      'update' => 0,
      'import_period' => 1800, // Refresh every 30 minutes by default.
      'expire_period' => 3600, // Expire every hour by default, this is a hidden setting.
      'import_on_create' => TRUE, // Import on create.
    );
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $form = array();
    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#description' => t('The name of this configuration.'),
      '#default_value' => $this->config['name'],
      '#required' => TRUE,
    );
    $form['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#description' => t('A description of this configuration.'),
      '#default_value' => $this->config['description'],
    );
    $form['content_type'] = array(
      '#type' => 'select',
      '#title' => t('Attach to content type'),
      '#description' => t('If you attach a configuration to a node you can use nodes for creating feeds on your site.'),
      '#options' => array('' => t('Use standalone form')) + node_get_types('names'),
      '#default_value' => $this->config['content_type'],
    );
    $period = drupal_map_assoc(array(0, 900, 1800, 3600, 10800, 21600, 43200, 86400, 259200, 604800, 2419200), 'format_interval');
    $period[FEEDS_SCHEDULE_NEVER] = t('Never');
    $period[0] = t('As often as possible');
    $form['import_period'] = array(
      '#type' => 'select',
      '#title' => t('Minimum refresh period'),
      '#options' => $period,
      '#description' => t('This is the minimum time that must elapse before a feed may be refreshed automatically.'),
      '#default_value' => $this->config['import_period'],
    );
    $form['import_on_create'] = array(
      '#type' => 'checkbox',
      '#title' => t('Import on create'),
      '#description' => t('Check if content should be imported at the moment of feed submission.'),
      '#default_value' => $this->config['import_on_create'],
    );
    return $form;
  }
}

/**
 * Helper, see FeedsDataProcessor class.
 */
function feeds_format_expire($timestamp) {
  if ($timestamp == FEEDS_EXPIRE_NEVER) {
    return t('Never');
  }
  return t('after !time', array('!time' => format_interval($timestamp)));
}