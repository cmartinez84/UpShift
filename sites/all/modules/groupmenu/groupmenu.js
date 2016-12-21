/**
 * @file
 * Group menu javascript functions.
 */

(function ($) {

  'use strict';

  Drupal.groupMenu = Drupal.groupMenu || {};

  Drupal.behaviors.groupMenuGroupswitch = {
    attach: function (context) {
      // Initialize variables and form.
      // Get original parent. We'll use this shortly.
      Drupal.groupMenu.originalParent = $('.menu-parent-select', context).val();
      Drupal.groupMenu.populateParentSelect(context);

      // Make sure the originalParent is set on page load.
      $('.menu-parent-select', context).val(Drupal.groupMenu.originalParent);
    }
  };

  /**
   * Populate the .menu-parent-select select with all available menus.
   *
   * This includes the groupmenus. This also sets as active the first menu for
   * the first selected group.
   *
   * @param context
   *   The context element.
   */
  Drupal.groupMenu.populateParentSelect = function (context) {
    // Remove all options from the select to rebuild it.
    $('.menu-parent-select option', context).remove();

    // Add any non groupmenus to the menu-parent-select menu.
    $.each(Drupal.settings.groupMenu.standard_parent_options, function (key, val) {
      $('.menu-parent-select', context).append($('<option>', {value: key, text: val}));
    });

    var parentToSetActive = Drupal.settings.groupMenu.group_id;
    var activeIsSet = Drupal.groupMenu.originalParent;

    // Add any groupmenus to the menu-parent-select menu.
    $.each(Drupal.settings.groupMenu.menus, function (menu_name, gid) {
      if (Drupal.settings.groupMenu.administer_group === true || parseInt(gid, 10) === Drupal.settings.groupMenu.group_id) {
        $.each(Drupal.settings.groupMenu.parent_options, function (key, val) {
          var parts = key.split(':');
          if (parts[0] === menu_name) {
            if (gid === parentToSetActive && activeIsSet === 0) {
              // Add option to Select and set as selected.
              $('.menu-parent-select', context).append($('<option>', {value: key, text: val, selected: 'selected'}));
              activeIsSet = 1;
            }
            else if (Drupal.settings.groupMenu.mlid !== 0 && Drupal.settings.groupMenu.mlid === parts[1]) {
              $('.menu-parent-select', context).append($('<option>', {value: key, text: val + ' [Current Menu Position]', disabled: 'disabled'}));
              // Don't add this item to parent list...
              // Set the parent so we don't lose our place.
              $('.menu-parent-select', context).val(activeIsSet);
            }
            else {
              // Add option to select.
              $('.menu-parent-select', context).append($('<option>', {value: key, text: val}));
            }
          }
        });
      }
    });
  };

}(jQuery));
