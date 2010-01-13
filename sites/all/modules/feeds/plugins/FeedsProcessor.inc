<?php
// $Id: FeedsProcessor.inc,v 1.4 2009/10/31 14:30:27 alexb Exp $

/**
 * Abstract class, defines interface for processors.
 *
 * @todo: declare an interface for mapping related functions.
 */
abstract class FeedsProcessor extends FeedsPlugin {

  /**
   * Process the result of the parser or previous processors.
   * Extending classes must implement this method.
   *
   * @param FeedsParserResult $parserResult
   *   The result of the parsing stage.
   * @param FeedsSource $source
   *   Source information about this import.
   *
   * @todo: Should it be execute()?
   */
  public abstract function process(FeedsParserResult $parserResult, FeedsSource $source);

  /**
   * Remove all stored results or stored results up to a certain time for this
   * configuration/this source.
   *
   * @param FeedsSource $source
   *   Source information for this expiry. Implementers should only delete items
   *   pertaining to this source. The preferred way of determining whether an
   *   item pertains to a certain souce is by using $source->feed_nid. It is the
   *   processor's responsibility to store the feed_nid of an imported item in
   *   the processing stage.
   *   @todo: pass in feed_nid instead of source?
   */
  public abstract function clear(FeedsSource $source);

  /**
   * Delete feed items younger than now - $time.
   *
   * @see FeedsDataProcessor::expire().
   *
   * @param $time
   *   If implemented, all items produced by this configuration that are older
   *   than FEEDS_REQUEST_TIME - $time
   *   If $time === NULL processor should use internal configuration.
   */
  public function expire($time = NULL) {}

  /**
   * Execute mapping on an item.
   */
  protected function map($source_item, $target_item = NULL) {

    // Static cache $targets as getMappingTargets() may be an expensive method.
    static $targets;
    if (empty($targets)) {
      $targets = $this->getMappingTargets();
    }
    $parser = feeds_importer($this->id)->parser;
    if (empty($target_item)) {
      $target_item = array();
    }

    /*
    This is where the actual mapping happens: For every mapping we envoke
    the parser's getSourceElement() method to retrieve the value of the source
    element and pass it to the processor's setTargetElement() to stick it
    on the right place of the target item.

    If the mapping specifies a callback method, use the callback instead of
    setTargetElement().
    */
    foreach ($this->config['mappings'] as $mapping) {
      $value = $parser->getSourceElement($source_item, $mapping['source']);

      if (is_array($targets[$mapping['target']]) && isset($targets[$mapping['target']]['callback']) && function_exists($targets[$mapping['target']]['callback'])) {
        $callback = $targets[$mapping['target']]['callback'];
        $callback($target_item, $mapping['target'], $value);
      }
      else {
        $this->setTargetElement($target_item, $mapping['target'], $value);
      }
    }
    return $target_item;
  }

  /**
   * Per default, don't support expiry. If processor supports expiry of imported
   * items, return the time after which items should be removed.
   */
  public function expiryTime() {
    return FEEDS_EXPIRE_NEVER;
  }

  /**
   * Declare default configuration.
   */
  public function configDefaults() {
    return array('mappings' => array());
  }

  /**
   * Add a mapping to existing mappings.
   *
   * @param $source
   *   A string that identifies a source element.
   * @param $target
   *   A string that identifies a target element.
   * @param $unique
   *   A boolean that defines whether the target value should be unique. If
   *   TRUE only one item with a given target value can exist on the local
   *   system. Compare with existingItemId() and uniqueTargets().
   */
  public function addMapping($source, $target, $unique = FALSE) {
    if (!empty($source) && !empty($target)) {
      $this->config['mappings'][] = array(
        'source' => $source,
        'target' => $target,
        'unique' => $unique,
      );
    }
  }

  /**
   * Set unique state of a mapping target.
   */
  public function setUnique($source, $target, $unique) {
    if (!empty($source) && !empty($target)) {
      foreach ($this->config['mappings'] as $k => $mapping) {
        if ($mapping['source'] == $source && $mapping['target'] == $target) {
          $this->config['mappings'][$k]['unique'] = $unique;
        }
      }
    }
  }

  /**
   * Remove a mapping.
   */
  public function removeMapping($source, $target) {
    foreach ($this->config['mappings'] as $k => $mapping) {
      if ($mapping['source'] == $source && $mapping['target'] == $target) {
        unset($this->config['mappings'][$k]);
      }
    }
    // Keep or keys clean.
    $this->config['mappings'] = array_values($this->config['mappings']);
  }

  /**
   * Get mappings.
   */
  public function getMappings() {
    return isset($this->config['mappings']) ? $this->config['mappings'] : array();
  }

  /**
   * Declare possible mapping targets.
   *
   * @return
   *   An array of mapping targets. Keys are paths to targets
   *   separated by ->, values are TRUE if target can be unique,
   *   FALSE otherwise.
   */
  public function getMappingTargets() {
    return array();
  }

  /**
   * Set target element.
   */
  public function setTargetElement(&$target_item, $target_element, $value) {
    $target_item[$target_element] = $value;
  }

  /**
   * Retrieve the target item's existing id if available. Otherwise return 0.
   *
   * @param $source_item
   *   A single item that has been aggregated from a feed.
   * @param FeedsSource $source
   *   The source information about this import.
   */
  protected function existingItemId($source_item, FeedsSource $source) {
    return 0;
  }

  /**
   * Utility function that iterates over a target array and retrieves all
   * sources that are unique.
   *
   * @param $source_item
   *   A feed item from a FeedsParserResult.
   *
   * @return
   *   An array where the keys are target field names and the values are the
   *   elements from the source item mapped to these targets.
   */
  protected function uniqueTargets($source_item) {
    $parser = feeds_importer($this->id)->parser;
    $targets = array();
    foreach ($this->config['mappings'] as $mapping) {
      if ($mapping['unique']) {
        // Invoke the parser's getSourceElement to retrieve the value for this
        // mapping's source.
        $targets[$mapping['target']] = $parser->getSourceElement($source_item, $mapping['source']);
      }
    }
    return $targets;
  }
}