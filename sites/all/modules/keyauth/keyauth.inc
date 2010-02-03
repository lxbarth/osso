<?php
/**
 * @file
 * Core Key Authentication functionality. Except queries, this is Drupal
 * independent.
 */

/**
 * Load a key.
 */
function keyauth_load($public_key) {
  ctools_include('export');
  $result = ctools_export_load_object('keyauth_keys', 'names', array('public_key' => $public_key));
  if (isset($result[$public_key])) {
    return $result[$public_key];
  }
}

/**
 * Load all keys.
 */
function keyauth_all_keys() {
  ctools_include('export');
  ctools_export_load_object_reset('keyauth_keys');
  $configs = ctools_export_load_object('keyauth_keys', 'all');
  return $configs;
}

/**
 * Look up a private key.
 */
function keyauth_key($public_key) {
  $keys = keyauth_all_keys();
  foreach ($keys as $p => $key) {
    if ($p == $public_key) {
      return $key->private_key;
    }
  }
}

/**
 * Insert new key set, public key and private key will be generated.
 *
 * @param $title
 *   Tite of key set. If empty, existing title will not be changed.
 * @param $public_key
 *   Public key. If empty, a new public key will generated. A key set is
 *   identified by its public key.
 * @param $private_key
 *   Private key, if omitted, a new private key will be generated.
 *
 * @return
 *   Complete key set as an array.
 */
function keyauth_insert($title, $public_key = NULL, $private_key = NULL) {
  $save = array(
    'title' => $title,
    'public_key' => empty($public_key) ? keyauth_random('md5') : $public_key,
    'private_key' => empty($private_key) ? keyauth_random('sha1') : $private_key,
  );
  drupal_write_record('keyauth_keys', $save);
  return $save;
}

/**
 * Update a key set.
 *
 * @param $public_key
 *   Public key.
 * @param $title
 *   Tite of key set.
 * @param $private_key
 *   Private key.
 *
 * @return
 *   Complete key set as an array.
 */
function keyauth_update($public_key, $title, $private_key) {
  $save = array(
    'title' => $title,
    'public_key' => $public_key,
    'public_key' => $public_key,
  );
  drupal_write_record('keyauth_keys', $save, 'public_key');
  return db_fetch_array(db_query("SELECT * FROM {keyauth_keys} WHERE public_key = '%s'", $public_key));
}

/**
 * Delete a key.
 */
function keyauth_delete($public_key) {
  db_query("DELETE FROM {keyauth_keys} WHERE public_key = '%s'", $public_key);
}

/**
 * Regenerate a private key for a given keyset.
 *
 * @param $public_key
 *   Public key.
 *
 * @return
 *   Complete key set as an array.
 */
function keyauth_regenerate($public_key) {
  $save = array(
    'public_key' => $public_key,
    'private_key' => keyauth_random('sha1'),
  );
  drupal_write_record('keyauth_keys', $save, 'public_key');
  return db_fetch_array(db_query("SELECT * FROM {keyauth_keys} WHERE public_key = '%s'", $public_key));
}

/**
 * Sign a message.
 *
 * @param $public_key
 *   The public key identifying a private key.
 * @param $message
 *   A string that is the message to hash.
 *
 * @return
 *   An array with the following keys:
 *   'nonce' - A random unique nonce.
 *   'timestamp' - The timestamp denoting the validity of the nonce.
 *   'hash' - The hash of message, nonce and timestamp.
 */
function keyauth_sign($public_key, $message) {
  $nonce = keyauth_random();
  $timestamp = time();
  $hash = hash_hmac(KEYAUTH_ALGORITHM, $message . $nonce . $timestamp, keyauth_key($public_key));
  return array(
    'nonce' => $nonce,
    'timestamp' => $timestamp,
    'hash' => $hash,
  );
}

/**
 * Verify a message.
 */
function keyauth_verify($public_key, $message, $nonce, $timestamp, $hash) {
  if ($private_key = keyauth_key($public_key)) {
    if (_keyauth_verify_nonce($public_key, $nonce, $timestamp)) {
      return $hash == hash_hmac(KEYAUTH_ALGORITHM, $message . $nonce . $timestamp, $private_key);
    }
  }
  return FALSE;
}

/**
 * Sign a URL.
 *
 * @todo Clean up, document.
 */
function keyauth_sign_url($public_key, $url) {
  // Normalize URL.
  if ($fragment = parse_url($url, PHP_URL_FRAGMENT)) {
    $url = str_replace("#$fragment", '', $url);
  }
  $scheme = parse_url($url, PHP_URL_SCHEME);
  $url = str_replace("$scheme://", '', $url);
  $url = trim($url, '?&');
  // Sign.
  list($nonce, $timestamp, $hash) = array_values(keyauth_sign($public_key, $url));
  // Assemble.
  $delimiter = parse_url($url, PHP_URL_QUERY) ? '&' : '?';
  $url .= $delimiter . 'hash='. urlencode($hash) .'&key='. urlencode($public_key) .'&nonce='. urlencode($nonce) .'&timestamp='. $timestamp;
  if ($fragment) {
    $url .= "#$fragment";
  }
  return "$scheme://$url";
}

/**
 * Verify a URL.
 *
 * @todo Clean up, document.
 */
function keyauth_verify_url($url) {
  // Normalize URL.
  if ($fragment = parse_url($url, PHP_URL_FRAGMENT)) {
    $url = str_replace("#$fragment", '', $url);
  }
  $scheme = parse_url($url, PHP_URL_SCHEME);
  $url = str_replace("$scheme://", '', $url);
  // Parse URL.
  $parsed = parse_url($url);
  if (!isset($parsed['query'])) {
    return FALSE;
  }
  parse_str($parsed['query'], $query);
  if (isset($query['hash']) && isset($query['key']) && isset($query['nonce']) && isset($query['timestamp'])) {
    $url = str_replace("hash={$query['hash']}&key={$query['key']}&nonce={$query['nonce']}&timestamp={$query['timestamp']}", '', $url);
    $hash = urldecode($query['hash']);
    $key = urldecode($query['key']);
    $nonce = urldecode($query['nonce']);
    $timestamp = $query['timestamp'];
  }
  else {
    return FALSE;
  }
  $url = trim($url, '?&');
  // Verify.
  return keyauth_verify($key, $url, $nonce, $timestamp, $hash);
}


/**
 * Provide a random, time dependent string.
 */
function keyauth_random($algorithm = 'md5') {
  return hash($algorithm, uniqid(rand(), true));
}

/**
 * Verify nonce.
 */
function _keyauth_verify_nonce($public_key, $nonce, $timestamp) {
  if (db_result(db_query("SELECT nonce FROM {keyauth_timestamp_nonce} WHERE public_key = '%s' AND nonce = '%s'", $public_key, $nonce))) {
    return FALSE;
  }
  $time = time();
  if ($timestamp > $time + KEYAUTH_EXPIRY || $timestamp < $time - KEYAUTH_EXPIRY) {
    return FALSE;
  }
  $save = array(
    'timestamp' => time(),
    'public_key' => $public_key,
    'nonce' => $nonce,
  );
  drupal_write_record('keyauth_timestamp_nonce', $save);
  return TRUE;
}