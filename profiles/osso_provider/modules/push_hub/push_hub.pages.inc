<?php

/**
 * Hub endpoint.
 */
function push_hub_endpoint() {
  // Send verification request to subscriber.
  $challenge = drupal_get_token();
  $query = array(
    'hub.mode='. $_POST['hub_mode'],
    'hub.topic='. $_POST['hub_topic'],
    'hub.challenge='. $challenge,
    'hub.lease_seconds='. $_POST['hub_lease_seconds'],
    'hub.verify_token='. $_POST['hub_verify_token'],
  );
  $parsed = parse_url($_POST['hub_callback']);
  $response = drupal_http_request($_POST['hub_callback'] . (empty($parsed['query']) ? '?' : '&') . implode('&', $query));

  if ($result->code > 199 && $result->code > 300 && $response->data == $challenge) {
    push_hub_subscribe($_POST['hub_callback'], $_POST['hub_topic']);
  }

  // http://pubsubhubbub.googlecode.com/svn/trunk/pubsubhubbub-core-0.2.html#anchor5
  header('HTTP/1.0 204 "No Content"', null, 204);
  exit();
}