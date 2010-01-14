<?php
// $Id: FeedsBatch.inc,v 1.2 2009/12/21 00:21:00 alexb Exp $

/**
 * A FeedsImportBatch is the actual content retrieved from a FeedsSource. On
 * import, it is created on the fetching stage and passed through the parsing
 * and processing stage where it is normalized and consumed.
 *
 * @see FeedsSource class
 * @see FeedsFetcher class
 */
class FeedsImportBatch {

  protected $url;
  protected $file_path;
  protected $raw;
  protected $items;
  protected $link;

  /**
   * Constructor.
   *
   * Either $url or $file_path must be given.
   */
  public function __construct($url = NULL, $file_path = NULL) {
    $this->url = $url;
    $this->file_path = $file_path;
    $this->items = array();
  }

  /**
   * @return
   *   The raw content of the feed.
   */
  public function getRaw() {
    if (empty($this->raw)) {
      // Prefer file.
      if ($this->file_path) {
        $this->raw = file_get_contents(realpath($this->file_path));
      }
      elseif ($this->url) {
        feeds_include_library('http_request.inc', 'http_request');
        $result = http_request_get($this->url);
        if ($result->code != 200) {
          throw new Exception(t('Download of @url failed with code !code.', array('@url' => $this->url, '!code' => $result->code)));
        }
        $this->raw = $result->data;
      }
    }
    return $this->raw;
  }

  /**
   * @return
   *   Path to the feed. This path is relative to Drupal's root directory.
   *   If the feed is not local, getFilePath downloads it to file directory.
   */
  public function getFilePath() {
    if (!isset($this->file_path)) {
      $dest = file_destination(file_directory_path() .'/feeds/'. get_class($this) .'_'. md5($this->url) .'_'. time(), FILE_EXISTS_RENAME);
      $this->file_path = file_save_data($this->getRaw(), $dest);
      if($this->file_path === 0) {
        throw new Exception(t('Cannot write content to %dest', array('%dest' => $dest)));
      }
    }
    return $this->file_path;
  }

  /**
   * @return
   *   URL to the document.
   */
  public function getURL() {
    if (!isset($this->url) && isset($this->file)) {
      return $_GLOBALS['base_url'] .'/'. $this->file;
    }
  }

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
    return isset($this->link) ? $this->link : $this->getURL();
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
   */
  public function setItems($items) {
    $this->items = $items;
  }

  /**
   * Add an item.
   */
  public function addItem($item) {
    $this->items[] = $item;
  }
}
