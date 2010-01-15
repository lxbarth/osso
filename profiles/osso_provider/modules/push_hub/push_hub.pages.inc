<?php

/**
 * Hub endpoint.
 */
function push_hub_endpoint() {
  if (push_hub()->verify($_POST)) {
    push_hub()->saveSubscription($_POST['hub_callback'], $_POST['hub_topic']);
  }


  // http://pubsubhubbub.googlecode.com/svn/trunk/pubsubhubbub-core-0.2.html#anchor5
  header('HTTP/1.0 204 "No Content"', null, 204);
  exit();
}