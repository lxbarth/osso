

OpenID Provider SSO
===================

OpenID Provider extension for Simple Sign-On among trusted sites.

Add trusted sites to a Drupal OpenID provider. Users enjoy a simplified sign on
process between trusted sites.

Note: this module does not provide true *Single* Sign-On functionality but a
largely simplified sign on experience for webs of trusted sites. Hence the name
*Simple* Sign-On.

Installation
============

Install this module and dependencies. In your site's settings.php file, define a
list of trusted Relying Parties:

// URL must have trailing slashes.
$conf['openid_provider_sso_rps'] = array(
  array(
    'realm' => 'http://example1.com/',
    'name' => 'The Example 1 Blog',
  ),
  array(
    'realm' => 'http://example2.com/',
    'name' => 'The Example 2 Blog',
  ),
);
