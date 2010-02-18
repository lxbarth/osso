
Drupal.behaviors.keyauth = function() {

  // Show keyauth boxes only when checked.
  if ($('#edit-use-keyauth').attr('checked')) {
    $('#edit-keyauth-public-wrapper').show();
  }
  else {
    $('#edit-keyauth-public-wrapper').hide();
  }
  $('#edit-use-keyauth').click(function() {
    if ($(this).attr('checked')) {
      $('#edit-keyauth-public-wrapper').show(200);
    }
    else {
      $('#edit-keyauth-public-wrapper').hide(200);
    }
  });
}
