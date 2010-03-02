OpenID SSO Demo (Relying Party)
-------------------------------

Drupal as OpenID provider in a web of trusted sites. Knowing the OpenID Provider
and all available Relying Parties allows for some simplifications of the sign-on
experience "Simple Sign-On". This profile uses OpenID Provider SSO to demo
Simple Sign-On.

Use together with OpenID SSO Demo (Provider) profile.

Installation
------------

NOTE: Use login/direct login directly to a relaying and avoid being sent to the
      SSO provider. This is useful when you need to login as an admin.

1. Run install profile, set up OpenID Provider information as described in OpenID
   SSO module README.

2. Go to admin/settings/openid-sso and enter OpenID Provider URL.

3. To distinguish particular sites better, use admin/build/themes/settings/hubnet
   to customize a site's color.