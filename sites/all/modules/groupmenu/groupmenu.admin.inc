<?php
/**
 * @file
 * Administrative functionality for groupmenu.
 */

/**
 * Form callback for Group Menu configuration.
 */
function groupmenu_config_form($form, &$form_state) {
  $form['groupmenu_max_menus_per_group'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of menus per group'),
    '#default_value' => variable_get('groupmenu_max_menus_per_group', 1),
    '#size' => 8,
    '#maxlength' => 5,
    '#required' => TRUE,
    '#description' => t("Enter 0 for no limit. Users with the 'administer menu' permission will be able to bypass this."),
  );

  $form['groupmenu_block_links'] = array(
    '#type' => 'checkbox',
    '#title' => t('Convert Group Menu block titles into links'),
    '#default_value' => variable_get('groupmenu_block_links', FALSE),
    '#description' => t('If enabled, Group Menu block titles will link to the group node.'),
  );

  // Automatic menu creation.
  $form['auto'] = array(
    '#type' => 'fieldset',
    '#title' => t('Automatic menu creation'),
  );

  $form['auto']['groupmenu_create_by_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically create menu for new Group'),
    '#default_value' => variable_get('groupmenu_create_by_default', FALSE),
    '#description' => t('If enabled, an Group Menu will be created by default when a new Group node is created.'),
  );

  // Visibility setting to hide Group Menus on selected pages.
  $form['hiding'] = array(
    '#type' => 'fieldset',
    '#title' => t('Admin page visibility'),
    '#description' => t("On sites with multiple Group Menus it might be preferable to hide them in places where you don't need them."),
  );

  $form['hiding']['groupmenu_show_nodetype'] = array(
    '#type' => 'checkbox',
    '#title' => t("Include Group Menus in node type menu settings"),
    '#default_value' => variable_get('groupmenu_show_nodetype', FALSE),
    '#description' => t("If disabled, Group Menus will be hidden from the node type config page."),
  );

  return system_settings_form($form);
}

/**
 * Validation for Group Menu config form.
 */
function groupmenu_config_form_validate($form, &$form_state) {
  $max_num = $form_state['values']['groupmenu_max_menus_per_group'];
  if (!is_numeric($max_num)) {
    form_set_error('groupmenu_max_menus_per_group', t('You must enter an integer for the maximum number of menus per group.'));
  }
  elseif ($max_num < 0) {
    form_set_error('groupmenu_max_menus_per_group', t('Maximum number of menus per group must be positive.'));
  }
}
