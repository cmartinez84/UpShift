(function ($) {

Drupal.behaviors.groupFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.node-form-group-information', context).drupalSetSummary(function (context) {
      var input = $('#edit-group-settings-group', context),
          select = $('#edit-group-settings-gid', context),
          value = input.length != 0 ? input.val() : select.val();

      if (value != '') {
        return $.trim(input.length != 0 ? value : select.children(':selected').text());
      }

      return Drupal.t('Not a group node');
    });
  }
};

})(jQuery);
