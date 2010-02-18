<?php
// $Id: FeedsBatch.inc,v 1.5 2010/02/18 16:01:19 alexb Exp $

/**
 * A FeedsBatch object holds the state of an import or clear batch. Used in
 * FeedsSource class.
 */
class FeedsBatch {
  // Public counters for easier access.
  public $total;
  public $created;
  public $updated;
  public $deleted;
  public function __construct() {
    $this->total = 0;
    $this->created = 0;
    $this->updated = 0;
    $this->deleted = 0;
  }
}

/**
 * A FeedsImportBatch wraps the actual content retrieved from a FeedsSource. On
 * import, it is created on the fetching stage and passed through the parsing
 * and processing stage where it is normalized and consumed.
 *
 * A Fetcher must return a FeedsImportBatch object on fetch(). To that end it
 * must use either one of the existing implementations of FeedsImportBatch
 * (FeedsFileBatch or FeedsHTTPBatch) or it must extend FeedsImportBatch and
 * implement at least
 *
 * - getRaw() returning the raw content from the source as a string and
 * - getFilePath() returning a path to a file containing the raw content from
 *   the source.
 *
 * A Parser must populate a FeedsImportBatch object through the set methods upon
 * parse(). For instance:
 *
 * $batch->setTitle('My imported document');
 * $batch->setItems($parsed_rows);
 *
 * Finally, a processor can work off the information produced on the parsing
 * stage by consuming items with $batch->shiftItem().
 *
 * while ($item = $batch->shiftItem()) {
 *   $object = $this->map($item);
 *   $object->save();
 * }
 *
 * Note: Knowledge of the internal structure of a single item in the $items
 * array is managed by the mapping API specified in FeedsParser class and
 * FeedsProcessor class.
 *
 * @see FeedsFileBatch
 * @see FeedsHTTPBatch
 */
abstract class FeedsImportBatch extends FeedsBatch {
  protected $title;
  protected $description;
  protected $link;
  protected $items;

  public function __construct() {
    $this->title = '';
    $this->description = '';
    $this->link = '';
    $this->items = array();
  }

  /**
   * @return
   *   The raw content from the source as a string.
   *
   * @throws Exception
   *   If an unexpected problem occurred.
   */
  public abstract function getRaw();

  /**
   * @return
   *   A path to a file containing the raw content as a source.
   *
   * @throws Exception
   *   If an unexpected problem occurred.
   */
  public abstract function getFilePath();

  /**
   * @return
   *   A string that is the feed's title.
   */
  public function getTitle() {
    return $this->title;
  }

  /**
   * @return
   *   A string that is the feed's description.
   */
  public function getDescription() {
    return $this->description;
  }

  /**
   * @return
   *   A string that is the link to the feed's site (not the actual URL of the
   *   feed). Falls back to URL if not available.
   */
  public function getLink() {
    return $this->link;
  }

  /**
   * @return
   *   Next available item or NULL if there is none. Every returned item is
   *   removed from the internal array.
   */
  public function shiftItem() {
    return array_shift($this->items);
  }

  /**
   * Set title.
   */
  public function setTitle($title) {
    $this->title = $title;
  }

  /**
   * Set description.
   */
  public function setDescription($description) {
    $this->description = $description;
  }

  /**
   * Set link.
   */
  public function setLink($link) {
    $this->link = $link;
  }

  /**
   * Set items.
   *
   * @param $items
   *   An array of the items in the feed. Cannot be NULL.
   */
  public function setItems($items) {
    $this->items = $items;
    $this->total = count($items);
  }

  /**
   * Add an item.
   */
  public function addItem($item) {
    $this->items[] = $item;
  }
}
