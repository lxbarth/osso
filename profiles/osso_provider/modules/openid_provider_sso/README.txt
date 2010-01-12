

OpenID Provider SSO
===================

OpenID Provider extension for Simple Sign-On among trusted sites.

Add trusted sites to a Drupal OpenID provider. Users enjoy a simplified sign on
process between trusted sites.

Note: this module does not provide true *Single* Sign-On functionality but a
largely simplified sign on experience for webs of trusted sites. Hence the name
*Simple* Sign-On.

It is strongly recommended to install this module on a fresh Drupal OpenID
Provider. Installing it on an existing OpenID provider will lead to undefined
results.

Installation
============

Install this module and dependencies. Go to admin/settings/openid-provider-sso
and enter known Relying Parties, for example:

Realm: http://example1.com/
Name: The Example 1 Blog

Realm: http://example2.com/
Name: The Example 2 Blog

All relying parties must be Drupal sites with the module OpenID SSO enabled and
configured.

Note that the realm requires a trailing slash.