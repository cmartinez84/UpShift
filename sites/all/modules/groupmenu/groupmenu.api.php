<?php
/**
 * @file
 * Documentation of the API functions exposed by this module.
 */

/**
 * Implements hook_groupmenu_admin_menu_overview_form_tableselect().
 *
 * Useful for other module that extend the functionality of the group menu admin
 * overview form.
 * When a module returns TRUE, the menu table will be rendered with checkboxes
 * in the left column.
 */
function hook_groupmenu_admin_menu_overview_form_tableselect() {
  return TRUE;
}
