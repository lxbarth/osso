Drupal.behaviors.osso_relying = function() {
  // Replace button with a link, make it look like on Provider.
  $('#openid-sso-form input.form-submit').addClass('processed').after(
      '<a href="#" onclick="return false;" class="login-submit">' + $('#openid-sso-form input.form-submit').val() + '</a>'
  ).hide();
  $('.login-submit').click( function () {
    $('#openid-sso-form').submit();
  });
}