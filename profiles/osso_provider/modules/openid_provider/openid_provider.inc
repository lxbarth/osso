<?php
// $Id: openid_provider.inc,v 1.3.2.6 2009/03/11 18:15:02 walkah Exp $

/**
 * Create an association with an RP
 *
 * @param array $request
 */
function openid_provider_association_response($request) {
  module_load_include('inc', 'openid');
  
  $session_type = $request['openid.session_type'];
  $assoc_type = $request['openid.assoc_type'];
  $dh_modulus = $request['openid.dh_modulus'];
  $dh_gen = $request['openid.dh_gen'];
  $dh_consumer_public = $request['openid.dh_consumer_public'];

  $assoc_handle = _openid_provider_nonce();
  $expires_in = variable_get('openid_provider_assoc_expires_in', '3600');

  // CLEAR STALE ASSOCIATIONS
  db_query("DELETE FROM {openid_provider_association} WHERE created + expires_in < %d", time());
  
  $response = array(
    'ns' => OPENID_NS_2_0,
    'session_type' => $session_type,
    'assoc_handle' => $assoc_handle,
    'assoc_type' => $assoc_type,
    'expires_in' => $expires_in
  );
  
  if ($session_type == 'DH-SHA1'
        || (($session_type == '' || $session_type == 'no-encryption')
            && $assoc_type == 'HMAC-SHA1')) {
    $num_bytes = 20;
    $algo = 'sha1';
  }
  elseif ($session_type == 'DH-SHA256'
        || (($session_type == '' || $session_type == 'no-encryption')
            && $assoc_type == 'HMAC-SHA256')) {
    $num_bytes = 32;
    $algo = 'sha256';
  }
  $secret = _openid_get_bytes($num_bytes);
  if ($session_type == '' || $session_type == 'no-encryption') {
    $mac_key = base64_encode(hash_hmac($algo, $response['assoc_handle'], $secret, true));
    $response['mac_key'] = $mac_key;
  }
  else {
    $dh_assoc = openid_provider_dh_assoc($request, $secret, $algo);
    $mac_key = base64_encode($secret);
    $response['dh_server_public'] = $dh_assoc['dh_server_public'];
    $response['enc_mac_key'] = $dh_assoc['enc_mac_key'];
  }
  // Save the association for reference when dealing
  // with future requests from the same RP.
  db_query("INSERT INTO {openid_provider_association} (assoc_handle, assoc_type, session_type, mac_key, created, expires_in) VALUES ('%s', '%s', '%s', '%s', %d, %d)",
           $assoc_handle, $assoc_type, $session_type, $mac_key, time(), $expires_in);

  $message = _openid_create_message($response);

  drupal_set_header('HTTP/1.1 200 OK');
  drupal_set_header("Content-Type: text/plain");
  print $message;
}

/**
 * Generate an association error response
 */
function openid_provider_association_error() {
  return array(
    'error' => '', // optional
    'error_code' => 'unsupported-type',
    'session_type' => '', // optional
    'assoc_type' => '' // optional
  );
}

/**
 * Generate an authentication response
 *
 * @param 
 */
function openid_provider_authentication_response($request) {
  global $user;

  // If the user is not yet logged in, redirect to the login page before continuing.
  if (!$user->uid) {
    $_SESSION['openid_provider']['request'] = $request;
    drupal_goto('user/login', 'destination=openid/provider/continue');
  }

  // Determine the realm (openid.trust_root in 1.x)
  $realm = (empty($request['openid.realm'])) ? $request['openid.trust_root'] : $request['openid.realm'];

  // Check for a directed identity request.
  if ($request['openid.identity'] == 'http://specs.openid.net/auth/2.0/identifier_select') {
    $identity = url(openid_provider_user_url($user->uid), array('absolute' => TRUE));
  }
  else {
    $identity = $request['openid.identity'];
    if ($identity != url(openid_provider_user_url($user->uid), array('absolute' => TRUE))) {
      $response = openid_provider_authentication_error($request['openid.mode']);
      openid_redirect($request['openid.return_to'], $response);
    }
  }

  $response = array(
    'openid.ns' => OPENID_NS_2_0,
    'openid.mode' => 'id_res',
    'openid.op_endpoint' => url('openid/provider', array('absolute' => TRUE)),
    'openid.identity' => $identity,
    'openid.claimed_id' => $identity,
    'openid.return_to' => $request['openid.return_to'],
    'openid.response_nonce' => _openid_provider_nonce(),
    'openid.assoc_handle' => $request['openid.assoc_handle'],
    'openid.sreg.nickname' => $user->name,
    'openid.sreg.email' => $user->mail
  );

  // Is the RP requesting Immediate or Indirect mode?
  if ($request['openid.mode'] == 'checkid_immediate') {
    // TODO
  }
  
  $parts = parse_url($request['openid.return_to']);
  if (isset($parts['query'])) {
    $query = $parts['query'];
    $q = _openid_get_params($query);
    foreach ($q as $key => $val) {
      $response[$key] = $val;
    }
  }

  // calling hook_openid so we can do response parsing and send any pertinent data back to the user
  $response = array_merge($response, module_invoke_all('openid_provider', 'response', $response, $request));
  
  $rp = _openid_provider_rp_load($user->uid, $realm);
  if ($rp->auto_release) {
    $response = _openid_provider_sign($response);
    _openid_provider_rp_save($user->uid, $realm, TRUE);
    return openid_redirect_http($response['openid.return_to'], $response); 
  }
  else {
    // Unset global post variable, otherwise FAPI will assume it has been 
    // submitted against openid_provider_form.
    unset($_POST);
    return drupal_get_form('openid_provider_form', $response, $realm);
  }
}

/**
 * Negative assertion
 */
function openid_provider_authentication_error($mode) {
  if ($mode == 'checkid_immediate') {
    return array(
      'openid.mode' => 'id_res',
      'openid.user_setup_url' => url('user/login', array('absolute' => TRUE))
    );
  }
  else { // checkid_setup
    return array(
      'openid.mode' => 'cancel'
    );
  }
}



function openid_provider_dh_assoc($request, $secret, $algo = 'sha1') {
  if (empty($request['openid.dh_consumer_public'])) {
    return FALSE;
  }
  
  if (isset($request['openid.dh_modulus'])) {
    $mod = _openid_dh_base64_to_long($request['openid.dh_modulus']);
  }
  else {
    $mod = OPENID_DH_DEFAULT_MOD;
  }

  if (isset($request['openid.dh_gen'])) {
    $gen = _openid_dh_base64_to_long($request['openid.dh_gen']);
  }
  else {
    $gen = OPENID_DH_DEFAULT_GEN;
  }

  $r = _openid_dh_rand($mod);
  $private = _openid_provider_add($r, 1);
  $public = _openid_provider_powmod($gen, $private, $mod);
  
  $cpub = _openid_dh_base64_to_long($request['openid.dh_consumer_public']);
  $shared = _openid_provider_powmod($cpub, $private, $mod);
  $mac_key = _openid_provider_dh_xorsecret($shared, $secret, $algo);
  $enc_mac_key = base64_encode($mac_key);  
  $spub64 = _openid_dh_long_to_base64($public);
  return array(
    'dh_server_public' => $spub64,
    'enc_mac_key' => $enc_mac_key
    );
}

/**
 * Is copy of _opend_dh_xorsecret() but uses PHP5 hash() function. Should be merged back into openid client
 * for D7.
 *
 * @param long $shared
 * @param string $secret
 * @param string $algo
 * @return binary string
 */
function _openid_provider_dh_xorsecret($shared, $secret, $algo = 'sha1') {
  $dh_shared_str = _openid_dh_long_to_binary($shared);
  $sha1_dh_shared = hash($algo, $dh_shared_str, true);
  $xsecret = "";
  for ($i = 0; $i < strlen($secret); $i++) {
    $xsecret .= chr(ord($secret[$i]) ^ ord($sha1_dh_shared[$i]));
  }
  return $xsecret;
}

// 9.2.2.2. Verifying Directly with the Identity Provider
// 9.2.2.2.2. Response Parameters
// Request is: Exact copies of all fields from the authentication response
function openid_provider_verification_response($request) {
  $is_valid = TRUE;
  
  // Use the request openid.assoc_handle to look up
  // how this message should be signed, based on
  // a previously-created association.
  $assoc = db_fetch_object(db_query("SELECT * FROM {openid_provider_association} WHERE assoc_handle = '%s'", 
    $request['openid.assoc_handle']));

  $signed_keys = explode(',', $request['openid.signed']);
  $signature = _openid_provider_signature($assoc, $request, $signed_keys);

  if ($signature != $request['openid.sig']) {
    $is_valid = FALSE;
  }

  if ($is_valid) {
    $response = array('is_valid' => 'true');
  }
  else {
    $response = array(
      'is_valid' => 'false',
      'invalidate_handle' => $request['openid.assoc_handle'] // optional, An association handle sent in the request
    );
  }

  $message = _openid_create_message($response);
  header("Content-Type: text/plain");
  print $message;  
}

function openid_provider_cancel_authentication_response($mode = 'checkid_immediate') {
  $response = array();
  if ($mode == 'checkid_immediate') {
    $response = array(
      'openid.ns' => OPENID_NS_2_0,
      'openid.mode' => 'id_res',
      'openid.user_setup_url' => url('user/login', array('absolute' => TRUE))
    );
  }
  else {
    $response = array('openid.module' => OPENID_NS_2_0, 'openid.mode' => 'cancel');
  }
  return $response;
}

function _openid_provider_rp_load($uid, $realm = NULL) {
  if ($realm) {
    return db_fetch_object(db_query("SELECT * FROM {openid_provider_relying_party} WHERE uid=%d AND realm='%s'", $uid, $realm));
  }
  else {
    $rps = array();
    $result = db_query("SELECT * FROM {openid_provider_relying_party} WHERE uid=%d ORDER BY last_time DESC", $uid);
    while ($rp = db_fetch_object($result)){
      $rps[] = $rp;
    }
    return $rps;
  }
}
  
function _openid_provider_rp_save($uid, $realm, $auto_release = FALSE) {
  $rpid = db_result(db_query("SELECT rpid FROM {openid_provider_relying_party} WHERE uid=%d AND realm='%s'", $uid, $realm));
  if ($rpid) {
    db_query("UPDATE {openid_provider_relying_party} SET auto_release=%d, last_time=%d WHERE rpid=%d", $auto_release, time(), $rpid);  
  }
  else {
    db_query("INSERT INTO {openid_provider_relying_party} (uid, realm, first_time, last_time, auto_release) VALUES (%d, '%s', %d, %d, %d)", $uid, $realm, time(), time(), $auto_release);
  }
}
function _openid_provider_nonce() {
  // YYYY-MM-DDThh:mm:ssTZD UTC, plus some optional extra unique chars
  return gmstrftime('%Y-%m-%dT%H:%M:%SZ') .
    chr(mt_rand(0, 25) + 65) .
    chr(mt_rand(0, 25) + 65) .
    chr(mt_rand(0, 25) + 65) .
    chr(mt_rand(0, 25) + 65);
}

function _openid_provider_sign($response) {
  module_load_include('inc', 'openid');
  
  $also_sign = array();
  $parts = parse_url($response['openid.return_to']);
  if (isset($parts['query'])) {
    $query = $parts['query'];
    $q = _openid_get_params($query);
    foreach ($q as $key => $val) {
      $also_sign[] = $key;
      $response[$key] = $val;
    }
  }

  $signed_keys = array('op_endpoint', 'return_to', 'response_nonce', 'assoc_handle', 'identity', 'claimed_id');
  $signed_keys = array_merge($signed_keys, module_invoke_all('openid_provider', 'signed', $response));
  $response['openid.signed'] = implode(',', $signed_keys);
  
  // Use the request openid.assoc_handle to look up
  // how this message should be signed, based on
  // a previously-created association.
  $assoc = db_fetch_object(db_query("SELECT * FROM {openid_provider_association} WHERE assoc_handle = '%s'", 
                                    $response['openid.assoc_handle']));
  
  // Generate signature for this message
  $response['openid.sig'] = _openid_provider_signature($assoc, $response, $signed_keys);
  return $response;
}

/**
 * Is copy from openid client but uses PHP5 only hash_hmac() function.
 *
 * @param object $association
 * @param array $message_array
 * @param array $keys_to_sign
 * @return string
 */
function _openid_provider_signature($association, $message_array, $keys_to_sign) {
  $signature = '';
  $sign_data = array();
  foreach ($keys_to_sign as $key) {
    if (isset($message_array['openid.'. $key])) {
      $sign_data[$key] = $message_array['openid.'. $key];
    }
  }
  $message = _openid_create_message($sign_data);
  $secret = base64_decode($association->mac_key);
  $signature = hash_hmac($association->assoc_type == 'HMAC-SHA256' ? 'sha256' : 'sha1', $message, $secret, true);
  return base64_encode($signature);
}

function _openid_provider_add($a, $b) {
  if (function_exists('gmp_add')) {
    return gmp_add($a, $b);
  }
  else if (function_exists('bcadd')) {
    return bcadd($a, $b);
  }
}

function _openid_provider_powmod($base, $exp, $mod) {
  if (function_exists('gmp_powm')) {
    return gmp_powm($base, $exp, $mod);
  }
  else if (function_exists('bcpowmod')) {
    return bcpowmod($base, $exp, $mod);
  }
}
