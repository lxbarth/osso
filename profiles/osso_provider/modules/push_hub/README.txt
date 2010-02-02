
PuSH Hub
--------

A PubSubHubbub Hub implementation for Drupal.

NOTES

* hub.lease_seconds requested by subscriber are ignored. Instead subscriptions
  are expired after PUSH_HUB_LEASE_SECONDS.
* Active removal of a subscription is not supported.
* Publisher protocol is not supported.

TODO

- Write publisher integration.
- See @todo comments in code.