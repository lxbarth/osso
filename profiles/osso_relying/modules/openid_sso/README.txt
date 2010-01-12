

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

It is strongly recommended to install this module on a fresh Drupal site.
Installing it on a site with existing users (other than user 1) will lead to
undefined results.


Installation
============

Install this module and dependencies.

Go to admin/settings/openid-sso and enter a trusted OpenID Provider. The
Provider must be a Drupal site with the module OpenID Provider SSO enabled and
configured.

Example:

Name: The Hub
URL: http://thehub.com/

Note that the URL requires a trailing slash.