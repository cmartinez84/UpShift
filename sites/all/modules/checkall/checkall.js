/**
 * @file
 */
(function ($) {
  Drupal.behaviors.checkAll = {
    attach: function(context, settings) {
      $('.form-checkall', context).once('checkAll', function() {
        Drupal.checkAll.attach(this, settings);
      });
    }
  };

  Drupal.checkAll = Drupal.checkAll || {
    strings: {
      checkAll: Drupal.t('Check all items in this group'),
      checkToggle: Drupal.t('Toggle the values of all items in this group'),
      checkNone: Drupal.t('Uncheck all items in this group')
    }
  };

  Drupal.checkAll.attach = function(checkBoxes,settings) {
    var groupClass = '';
    // Find the class that connects the checkboxes group with its individual checkbox elements.
    if (settings.checkall && settings.checkall.groups) {
      // If there's just one group, then it comes in the form of a string.
      // So in this case, we need to convert the setting into an array.
      if (typeof settings.checkall.groups == 'string') {
        settings.checkall.groups = [settings.checkall.groups];
      }
      var groupClasses = checkBoxes.className.toString().split(/\s+/);
      for (var i = 0; i < groupClasses.length; i++) {
        for (var j = 0; j < settings.checkall.groups.length; j++) {
          if (groupClasses[i] == settings.checkall.groups[j]) {
            groupClass = groupClasses[i];
            break;
          }
        }
        if (groupClass.length > 0) {
          break;
        }
      }
    }

    if (groupClass.length > 0 && $('th.'+ groupClass).size() > 0) {
      // The behavior is attached to all checkbox items that share the
      // same CSS class that was used in the '#checkall' property of
      // the FormAPI checkboxes element.
      Drupal.checkAll.attachByClass(groupClass);
    }
    else {
      // The behavior is attached to all checkbox items that are
      // children of the checkboxes wrapper.
      Drupal.checkAll.attachByGroup(checkBoxes, settings);
    }
  };

  Drupal.checkAll.attachByClass = function(groupClass) {
    // Adjust the state and the title attribute of header checkbox depending
    // on whether all checkboxes in the same group are checked or not.
    function setHeaderStatus() {
      var checkbox = $('input:checkbox');

      if (checkbox.filter('.'+ groupClass).size() == checkbox.filter('.'+ groupClass).filter(':checked').size()) {
        $('th.'+ groupClass +' input:checkbox').filter(':not(:checked)').attr({'checked': true, 'title': Drupal.checkAll.strings.checkNone}).trigger('change');
      }
      else {
        $('th.'+ groupClass +' input:checkbox').filter(':checked').attr({'checked': false, 'title': Drupal.checkAll.strings.checkAll}).trigger('change');
      }
    }

    // Attach a check/uncheck checkbox to table header cell.
    $('th.'+ groupClass).append($('<div></div>')).append(
      $('<input type="checkbox" class="form-checkbox" />').attr('title', Drupal.checkAll.strings.checkAll).click(function(event) {
        var state = event.target.checked;
        $(this).attr('title', state ? Drupal.checkAll.strings.checkNone : Drupal.checkAll.strings.checkAll);
        $('input:checkbox').filter('.'+ groupClass).filter(state ? ':not(:checked)' : ':checked').attr('checked', state).trigger('change');
      })
    );

    // Attach a click event to each checkbox so that we can re-evaluate the
    // state of the header checkbox when all items are checked/unchecked.
    $('input:checkbox').filter('.'+ groupClass).click(function() {
      setHeaderStatus();
    });

    // Adjust header checkbox when the behavior is attached.
    setHeaderStatus();
  };

  Drupal.checkAll.attachByGroup = function(checkBoxes,settings) {
    $(checkBoxes).prepend('<ul class="checkall-action"></ul>');
    $checkboxes = $(checkBoxes).children('.checkall-action');
    if (settings.checkall.show_uncheck_all) {
      $checkboxes
        .prepend($('<li><a href="#" class="checkall-uncheck-all">'+ Drupal.t('Uncheck all') +'</a></li>').attr('title', Drupal.checkAll.strings.checkNone).click(function() {
          $('input:checkbox:checked', checkBoxes).click();
          return false;
        }));
    }
    if (settings.checkall.show_toggle) {
      $checkboxes
        .prepend($('<li><a href="#" class="checkall-toggle">'+ Drupal.t('Toggle') +'</a></li>').attr('title', Drupal.checkAll.strings.checkToggle).click(function() {
          var checkedBoxes   = $('input:checked', checkBoxes),
              uncheckedBoxes = $('input:not(:checked)', checkBoxes);
          $(checkedBoxes).click();
          $(uncheckedBoxes).click();
          return false;
        }));
    }
    if (settings.checkall.show_check_all) {
      $checkboxes
        .prepend($('<li><a href="#" class="checkall-check-all">'+ Drupal.t('Check all') +'</a></li>').attr('title', Drupal.checkAll.strings.checkAll).click(function() {
          $('input:checkbox:not(:checked)', checkBoxes).click();
          return false;
        }));
    }
  };
})(jQuery);
