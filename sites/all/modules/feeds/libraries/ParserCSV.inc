<?php
// $Id: ParserCSV.inc,v 1.2 2009/11/05 18:26:27 alexb Exp $

/**
 * Contains CSV Parser.
 * Functions in this file are independent of the Feeds specific implementation.
 * Thanks to jpetso http://drupal.org/user/56020 for most of the code in this
 * file.
 */

/**
 * Text lines from file iterator.
 */
class ParserCSVIterator implements Iterator {
  private $handle;
  private $currentLine;

  public function __construct($filepath) {
    $this->handle = fopen($filepath, 'r');
    $this->currentLine = NULL;
  }

  function __destruct() {
    if ($this->handle) {
      fclose($this->handle);
    }
  }

  public function rewind() {
    if ($this->handle) {
      fseek($this->handle, 0);
      $this->next();
    }
  }

  public function next() {
    if ($this->handle) {
      $this->currentLine = feof($this->handle) ? NULL : fgets($this->handle);
      return $this->currentLine;
    }
  }

  public function valid() {
    return isset($this->currentLine);
  }

  public function current() {
    return $this->currentLine;
  }

  public function key() {
    return 'line';
  }
}

/**
 * Functionality to parse CSV files into a two dimensional array.
 */
class ParserCSV {
  private $delimiter;
  private $skipFirstLine;
  private $columnNames;
  private $timeout;
  private $timeoutReached;

  public function __construct() {
    $this->delimiter = ',';
    $this->skipFirstLine = FALSE;
    $this->columnNames = FALSE;
    $this->timeout = FALSE;
    $this->timeoutReached = FALSE;
  }

  /**
   * Set the column delimiter string.
   * By default, the comma (',') is used as delimiter.
   */
  public function setDelimiter($delimiter) {
    $this->delimiter = $delimiter;
  }

  /**
   * Set this to TRUE if the parser should skip the first line of the CSV text,
   * which might be desired if the first line contains the column names.
   * By default, this is set to FALSE and the first line is not skipped.
   */
  public function setSkipFirstLine($skipFirstLine) {
    $this->skipFirstLine = $skipFirstLine;
  }

  /**
   * Specify an array of column names if you know them in advance, or FALSE
   * (which is the default) to unset any prior column names. If no column names
   * are set, the parser will put each row into a simple numerically indexed
   * array. If column names are given, the parser will create arrays with
   * these column names as array keys instead.
   */
  public function setColumnNames($columnNames) {
    $this->columnNames = $columnNames;
  }

  /**
   * Define the time (in milliseconds) after which the parser stops parsing,
   * even if it has not yet finished processing the CSV data. If the timeout
   * has been reached before parsing is done, the parse() method will return
   * an incomplete list of rows - a single row will never be cut off in the
   * middle, though. By default, no timeout (@p $timeout == FALSE) is defined.
   *
   * You can check if the timeout has been reached by calling the
   * timeoutReached() method after parse() has been called.
   */
  public function setTimeout($timeout) {
    $this->timeout = $timeout;
  }

  /**
   * After calling the parse() method, determine if the timeout (set by the
   * setTimeout() method) has been reached.
   */
  public function timeoutReached() {
    return $this->timeoutReached;
  }

  /**
   * Parse CSV files into a two dimensional array.
   *
   * @param Iterator $lineIterator
   *   An Iterator object that yields line strings, e.g. ParserCSVIterator.
   * @return
   *   Two dimensional array that contains the data in the CSV file.
   */
  public function parse(Iterator $lineIterator) {
    $skipLine = $this->skipFirstLine;
    $rows = array();

    $this->timeoutReached = FALSE;
    $maxTime = empty($this->timeout) ? FALSE : (microtime() + $this->timeout);

    for ($lineIterator->rewind(); $lineIterator->valid(); $lineIterator->next()) {
      // If the timeout has been reached, quit parsing even if we're not yet done.
      if (!empty($maxTime) && microtime() > $maxTime) {
        $this->timeoutReached = TRUE;
        break;
      }

      // Make really sure we've got lines without trailing newlines.
      $line = trim($lineIterator->current(), "\r\n");

      // Skip empty lines.
      if (empty($line)) {
        continue;
      }
      // If the first line contains column names, skip it.
      if ($skipLine) {
        $skipLine = FALSE;
        continue;
      }

      // The actual parser. explode() is unfortunately not suitable because the
      // delimiter might be located inside a quoted field, and that would break
      // the field and/or require additional effort to re-join the fields.
      $quoted = FALSE;
      $currentIndex = 0;
      $currentField = '';
      $fields = array();

      while ($currentIndex <= strlen($line)) {
        if ($quoted) {
          $nextQuoteIndex = strpos($line, '"', $currentIndex);

          if ($nextQuoteIndex === FALSE) {
            // There's a line break before the quote is closed, so fetch the
            // next line and start from there.
            $currentField .= substr($line, $currentIndex);
            $lineIterator->next();

            if (!$lineIterator->valid()) {
              // Whoa, an unclosed quote! Well whatever, let's just ignore
              // that shortcoming and record it nevertheless.
              $fields[] = $currentField;
              break;
            }
            // Ok, so, on with fetching the next line, as mentioned above.
            $currentField .= "\n";
            $line = trim($lineIterator->current(), "\r\n");
            $currentIndex = 0;
            continue;
          }

          // There's actually another quote in this line...
          // find out whether it's escaped or not.
          $currentField .= substr($line, $currentIndex, $nextQuoteIndex - $currentIndex);

          if (isset($line[$nextQuoteIndex + 1]) && $line[$nextQuoteIndex + 1] === '"') {
            // Escaped quote, add a single one to the field and proceed quoted.
            $currentField .= '"';
            $currentIndex = $nextQuoteIndex + 2;
          }
          else {
            // End of the quoted section, close the quote and let the
            // $quoted == FALSE block finalize the field.
            $quoted = FALSE;
            $currentIndex = $nextQuoteIndex + 1;
          }
        }
        else { // $quoted == FALSE
          // First, let's find out where the next character of interest is.
          $nextQuoteIndex = strpos($line, '"', $currentIndex);
          $nextDelimiterIndex = strpos($line, $this->delimiter, $currentIndex);

          if ($nextQuoteIndex === FALSE) {
            $nextIndex = $nextDelimiterIndex;
          }
          elseif ($nextDelimiterIndex === FALSE) {
            $nextIndex = $nextQuoteIndex;
          }
          else {
            $nextIndex = min($nextQuoteIndex, $nextDelimiterIndex);
          }

          if ($nextIndex === FALSE) {
            // This line is done, add the rest of it as last field.
            $currentField .= substr($line, $currentIndex);
            $fields[] = $currentField;
            break;
          }
          elseif ($line[$nextIndex] === $this->delimiter[0]) {
            $length = ($nextIndex + strlen($this->delimiter) - 1) - $currentIndex;
            $currentField .= substr($line, $currentIndex, $length);
            $fields[] = $currentField;
            $currentField = '';
            $currentIndex += $length + 1;
            // Continue with the next field.
          }
          else { // $line[$nextIndex] == '"'
            $quoted = TRUE;
            $currentField .= substr($line, $currentIndex, $nextIndex - $currentIndex);
            $currentIndex = $nextIndex + 1;
            // Continue this field in the $quoted == TRUE block.
          }
        }
      }
      // End of CSV parser. We've now got all the fields of the line as strings
      // in the $fields array.

      if (empty($this->columnNames)) {
        $row = $fields;
      }
      else {
        $row = array();
        foreach ($this->columnNames as $columnName) {
          $field = array_shift($fields);
          $row[$columnName] = isset($field) ? $field : '';
        }
      }
      $rows[] = $row;
    }
    return $rows;
  }
}