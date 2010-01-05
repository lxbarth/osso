/**
 * @file
 * Javascript behaviors for OpenID Provider SSO.
 */

Drupal.behaviors.openid_provider_sso = function() {
  if (Drupal.settings.openid_provider_sso_destination) {
    var query = "destination=" + Drupal.settings.openid_provider_sso_destination;
    var paths = new Array(
      'user',
      'user/register',
      'user/login',
      'user/password'
    );
    var base = Drupal.settings.basePath;
    for (i = 0; i < paths.length; i++) {
      $("a[href='" + base + "?q=" + paths[i] + "']").attr('href', base + '?q=' + paths[i] + '&' + query);
      $("a[href='" + base + "" + paths[i] + "']").attr('href', base + paths[i] + '?' + query);
    }
  }
}