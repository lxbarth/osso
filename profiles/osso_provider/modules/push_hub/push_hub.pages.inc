<?php

/**
 * Hub endpoint.
 */
function push_hub_endpoint() {
  push_hub()->subscribe($_POST);
}