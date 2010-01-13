<?php
// $Id: FeedsSyndicationParser.inc,v 1.11 2010/01/12 16:28:02 alexb Exp $

/**
 * Enclosure element, can be part of the result array.
 */
class FeedsEnclosure extends FeedsElement {
  protected $mime_type;
  protected $file;

  /**
   * Constructor, requires MIME type.
   */
  public function __construct($value, $mime_type) {
    parent::__construct($value);
    $this->mime_type = $mime_type;
  }

  /**
   * @return
   *   MIME type of return value of getValue().
   */
  public function getMIMEType() {
    return $this->mime_type;
  }

  /**
   * @return
   *   The content of the referenced resource.
   */
  public function getContent() {
    feeds_include_library('http_request.inc', 'http_request');
    $result = http_request_get($this->getValue());
    if ($result->code != 200) {
      throw new Exception(t('Download of @url failed with code !code.', array('@url' => $this->getValue(), '!code' => $result->code)));
    }
    return $result->data;
  }

  /**
   * @return
   *   The file path to the downloaded resource referenced by the enclosure.
   *   Downloads resource if not downloaded yet.
   *
   * @todo Get file extension from mime_type.
   * @todo This is not concurrency safe.
   */
  public function getFile() {
    if(!isset($this->file)) {
      $dest = file_destination(file_directory_temp() .'/'. get_class($this) .'-'. basename($this->getValue()), FILE_EXISTS_RENAME);
      if (ini_get('allow_url_fopen')) {
        $this->file = copy($this->getValue(), $dest) ? $dest : 0;
      }
      else {
        $this->file = file_save_data($this->getContent(), $dest);
      }
      if ($this->file === 0) {
        throw new Exception(t('Cannot write content to %dest', array('%dest' => $dest)));
      }
    }
    return $this->file;
  }
}

/**
 * Class definition for Common Syndication Parser.
 *
 * Parses RSS and Atom feeds.
 */
class FeedsSyndicationParser extends FeedsParser {

  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    feeds_include_library('common_syndication_parser.inc', 'common_syndication_parser');
    $result = common_syndication_parser_parse($batch->getRaw());
    $batch->setTitle($result['title']);
    $batch->setDescription($result['description']);
    $batch->setLink($result['link']);
    $batch->setItems($result['items']);
  }

  /**
   * Return mapping sources.
   *
   * At a future point, we could expose data type information here,
   * storage systems like Data module could use this information to store
   * parsed data automatically in fields with a correct field type.
   */
  public function getMappingSources() {
    return array(
      'title' => array(
        'name' => t('Title'),
        'description' => t('Title of the feed item.'),
      ),
      'description' => array(
        'name' => t('Description'),
        'description' => t('Description of the feed item.'),
      ),
      'author_name' => array(
        'name' => t('Author name'),
        'description' => t('Name of the feed item\'s author.'),
      ),
      'timestamp' => array(
        'name' => t('Published date'),
        'description' => t('Published date as UNIX time GMT of the feed item.'),
      ),
      'url' => array(
        'name' => t('Item URL (link)'),
        'description' => t('URL of the feed item.'),
      ),
      'guid' => array(
        'name' => t('Item GUID'),
        'description' => t('Global Unique Identifier of the feed item.'),
      ),
      'tags' => array(
        'name' => t('Categories'),
        'description' => t('An array of categories that have been assigned to the feed item.'),
      ),
     );
  }
}