

Key Authentication
------------------

Provides an API and a basic UI for key based authentication. Modeled after
services_keyauth module.


Installation
------------

Install module, go to admin/build/keys and create a new key or add an existing
key.


Basic usage
-----------

1) Generate a key through UI or through calling keyauth_save()

$key = keyauth_save('Test key');

2) Sign a message

keyauth_include();
list($nonce, $timestamp, $hash) = array_values(keyauth_sign($key['public_key'], 'Lorem ipsum'));

3) Verify a message

keyauth_include();
$verified = keyauth_verify($key['public_key'], 'Lorem ipsum', $nonce, $timestamp, $hash);


Sign URLs
---------

Key Authentication module comes with helper functions to sign URLs:

keyauth_include();
$signed = keyauth_sign_url($key['public_key'], 'http://example.com');
$verified = keyauth_verify_url($key['public_key'], $signed);


"Real world" example
--------------------

1) Set up Key Authorization on two Drupal sites, create a new key on one of the
   sites and add the same key (public key and private key) to the other site.
2) Create a module 'myresource' that exposes a path that should be protected, use
   keyauth_verify_url in the path's access check. Enable the module on one of
   the sites.
3) Create a module 'myconsumer' that pulls the protected path of 'myresource'
   using drupal_http_request() and keyauth_sign().