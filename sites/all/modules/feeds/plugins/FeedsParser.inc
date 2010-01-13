<?php
// $Id: FeedsParser.inc,v 1.5 2010/01/12 16:28:02 alexb Exp $

/**
 * Defines an element of a parsed result. Such an element can be a simple type,
 * a complex type (derived from FeedsElement) or an array of either.
 *
 * @see FeedsEnclosure
 */
class FeedsElement {
  // The standard value of this element. This value can contain be a simple type,
  // a FeedsElement or an array of either.
  protected $value;

  /**
   * Constructor.
   */
  public function __construct($value) {
    $this->value = $value;
  }

  /**
   * @return
   *   Standard value of this FeedsElement.
   */
  public function getValue() {
    return $this->value;
  }

  /**
   * @return
   *   A string representation of this element.
   */
  public function __toString() {
    if (is_array($this->value)) {
      return 'Array';
    }
    if (is_object($this->value)) {
      return 'Object';
    }
    return (string) $this->value;
  }
}

/**
 * Abstract class, defines interface for parsers.
 */
abstract class FeedsParser extends FeedsPlugin {

  /**
   * Parse content fetched by fetcher.
   *
   * Extending classes must implement this method.
   *
   * @param $batch
   *   FeedsImportBatch returned by fetcher.
   * @param FeedsSource $source
   *   Source information.
   */
  public abstract function parse(FeedsImportBatch $batch, FeedsSource $source);

  /**
   * Clear all caches for results for given source.
   *
   * @param FeedsSource $source
   *   Source information for this expiry. Implementers can choose to only clear
   *   caches pertaining to this source.
   */
  public function clear(FeedsSource $source) {}

  /**
   * Declare the possible mapping sources that this parser produces.
   *
   * @return
   *   An array of mapping sources, or FALSE if the sources can be defined by
   *   typing a value in a text field.
   *
   *   Example:
   *   array(
   *     'title' => t('Title'),
   *     'created' => t('Published date'),
   *     'url' => t('Feed item URL'),
   *     'guid' => t('Feed item GUID'),
   *   )
   */
  public function getMappingSources() {
    return FALSE;
  }

  /**
   * Get an element identified by $element_key of the given item.
   * The element key corresponds to the values in the array returned by
   * FeedsParser::getMappingSources().
   */
  public function getSourceElement($item, $element_key) {
    return isset($item[$element_key]) ? $item[$element_key] : '';
  }
}