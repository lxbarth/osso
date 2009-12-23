<?php
// $Id: DataHandler.inc,v 1.1.2.4 2009/10/12 19:52:51 alexb Exp $
/**
 * @file
 * Definition of DataHandler class.
 */

/**
 * Simple access methods to table data. Can be used on any table, not just Data
 * managed tables.
 */
class DataHandler {

  // Holds the name of the table that this handler is responsible for.
  protected $table;

  /**
   * Constructor, call indirectly through DataHandler::instance();
   */
  protected function __construct($table) {
    $this->table = $table;
  }

  /**
   * Instantiate a DataHandler object.
   *
   * @param $table
   *   The name of the table to access with this DataHandler object.
   */
  public static function instance($table) {
    static $handlers;
    if (!isset($handlers[$table])) {
      $handlers[$table] = new DataHandler($table);
    }
    return $handlers[$table];
  }

  /**
   * Getter.
   */
  public function __get($name) {
    return $this->$name;
  }

  /**
   * Load a record.
   */
  public function load($keys) {
    $where = $values = array();
    $schema = drupal_get_schema($this->table);
    $fields = $schema['fields'];
    foreach ($keys as $key => $value) {
      // Return if a key does not exist.
      if (!isset($fields[$key]['type'])) {
        return FALSE;
      }
      $where[] = db_escape_string($key) ." = ". db_type_placeholder($fields[$key]['type']);
      $values[] = $value;
    }

    if (!empty($where)) {
      $result = db_query('SELECT * FROM {'. db_escape_table($this->table) .'} WHERE '. implode(' AND ', $where), $values);
      $results = array();
      while ($row = db_fetch_array($result)) {
        $results[] = $row;
      }
      return count($results) ? $results : FALSE;
    }
    return FALSE;
  }

  /**
   * Insert a record.
   *
   * @see drupal_write_record().
   *
   * @param $record
   *   An array that is the record to save to this handler's table.
   */
  public function insert(&$record) {
    return drupal_write_record($this->table, $record);
  }

  /**
   * Update a record.
   *
   * @see drupal_write_record().
   *
   * @param $record
   *   An array that is the record to save to this handler's table.
   * @param $update
   *   A string or an array of strings that defines the keys to use for
   *   this update.
   */
  public function update(&$record, $update) {
    return drupal_write_record($this->table, $record, $update);
  }

  /**
   * Save one or more records to the table.
   *
   * If $update is given, method will try to update before.
   *
   * This method is more comfortable, but slower than using insert() or
   * update().
   *
   * @param $record
   *   An array that is the record to save to this handler's table.
   * @param
   *   A string or an array of strings that defines the keys to use for
   *   this update.
   */
  public function save(&$record, $update = array()) {
    if (is_string($update) || count($update)) {
      if (SAVED_UPDATED != drupal_write_record($this->table, $record, $update)) {
        return drupal_write_record($this->table, $record);
      }
    }
    return drupal_write_record($this->table, $record);
  }

  /**
   * Delete one or more records from the table.
   */
  public function delete($keys) {
    $where = $values = array();
    $schema = drupal_get_schema($this->table);
    $fields = $schema['fields'];
    foreach ($keys as $key => $value) {
      $where[] = db_escape_string($key) ." = ". db_type_placeholder($fields[$key]['type']);
      $values[] = $value;
    }
    if (!empty($where)) {
      db_query('DELETE FROM {'. db_escape_table($this->table) .'} WHERE '. implode(' AND ', $where), $values);
    }
    return db_affected_rows();
  }

  /**
   * Empty data table.
   */
  public function truncate() {
    db_query('TRUNCATE TABLE {'. db_escape_table($this->table) .'}');
  }
}
