/**
 * @file
 * Javascript behaviors for OpenID Provider SSO.
 */

Drupal.behaviors.openid_provider_sso = function() {
  if (Drupal.settings.destination) {
    var base = Drupal.settings.basePath;
    var destination = "destination=" + Drupal.settings.destination;
    $("a[href='" + base + "?q=user']").attr('href', base + '?q=user&' + destination);
    $("a[href='" + base + "user']").attr('href', base + 'user?' + destination);
    $("a[href='" + base + "?q=user/register']").attr('href', base + '?q=user/register&' + destination);
    $("a[href='" + base + "user/register']").attr('href', base + 'user/register?' + destination);
    $("a[href='" + base + "?q=user/login']").attr('href', base + '?q=user/login&' + destination);
    $("a[href='" + base + "user/login']").attr('href', base + 'user/login?' + destination);
    $("a[href='" + base + "?q=user/password']").attr('href', base + '?q=user/password&' + destination);
    $("a[href='" + base + "user/password']").attr('href', base + 'user/password?' + destination);
  }
}