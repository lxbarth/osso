<?php

/**
 * Hub endpoint.
 */
function push_hub_endpoint() {
  error_log(print_r($_POST, true));
  // http://pubsubhubbub.googlecode.com/svn/trunk/pubsubhubbub-core-0.2.html#anchor5
  header('HTTP/1.0 204 "No Content"', null, 204);
  exit();
}