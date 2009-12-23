

HEADS UP
========

This module moves your standard log in page to login/direct/ and places at its
previous location (user/) a redirect to the configured OpenID Provider.


OpenID SSO
==========

OpenID based Simple Sign-On among trusted sites.

Add a trusted Drupal OpenID provider to a Drupal site. Users enjoy a simplified
sign on process between trusted sites.

Assumes that the OpenID Provider in place is a Drupal site with OpenID Provider
SSO module enabled.

Note: this module does not provide true *Single* Sign-On functionality but a
largely simplified sign on experience for webs of trusted sites. Hence the name
*Simple* Sign-On.


Installation
============

Install this module and dependencies.

In your site's settings.php file, define a trusted OpenID Provider. OpenID
Provider must have OpenID Provider SSO enabled.

$conf['openid_sso_provider'] = array(
  'url' => 'http://localhost/op/', // Must have trailing slashes
  'name' => 'SSO Web', // Mostly always the Provider's site name.
);


