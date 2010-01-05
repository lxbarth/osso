/**
 * @file
 * Javascript behaviors for OpenID Provider SSO.
 */

Drupal.behaviors.openid_provider_sso = function() {
  var query = '';
  if (Drupal.settings.openid_provider_sso_destination) {
    query = "destination=" + Drupal.settings.openid_provider_sso_destination;
  }
  else if (Drupal.settings.openid_provider_sso.realm) {
    query = "realm=" + Drupal.settings.openid_provider_sso.realm;
  }
  if (query.length > 0) {
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
//    $("a[href='" + base + "?q=user']").attr('href', base + '?q=user&' + destination);
//    $("a[href='" + base + "user']").attr('href', base + 'user?' + destination);
//    $("a[href='" + base + "?q=user/register']").attr('href', base + '?q=user/register&' + destination);
//    $("a[href='" + base + "user/register']").attr('href', base + 'user/register?' + destination);
//    $("a[href='" + base + "?q=user/login']").attr('href', base + '?q=user/login&' + destination);
//    $("a[href='" + base + "user/login']").attr('href', base + 'user/login?' + destination);
//    $("a[href='" + base + "?q=user/password']").attr('href', base + '?q=user/password&' + destination);
//    $("a[href='" + base + "user/password']").attr('href', base + 'user/password?' + destination);
}