Drupal.behaviors.hubnet_settings = function (context) {

  // Add Farbtastic
  var target = new Array($('#edit-header-color', context), $('#edit-header-color-dark', context));
  for (var i = 0; i < target.length; i++) {
    var farb = $.farbtastic('div#hubnet-colorpicker-' + i, target[i]);
    target[i]
      .focus(function() {
        $('div#hubnet-colorpicker-' + i).show('medium');
      })
      .blur(function() {
        $('div#hubnet-colorpicker-' + i).hide('medium');
      });
  }
};