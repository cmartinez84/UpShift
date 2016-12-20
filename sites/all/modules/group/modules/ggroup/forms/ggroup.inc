<?php
/**
 * @file
 * Subgroup creation form.
 *
 * This file contains hook_form_alter() variants even though it isn't always
 * included from the .module file. The reason for this is that the forms they
 * alter reside in this file and are loaded by hook_menu() anyway.
 */

/**
 * Generate the subgroup creation form.
 */
function ggroup_add_subgroup_form($group, GroupType $group_type) {
  $form_state = array();

  $subgroup = entity_create('group', array(
    'group' => $group->gid,
    'type' => $group_type->name,
  ));

  // Group creation / editing form lives in a separate file.
  form_load_include($form_state, 'inc', 'group', 'forms/group');
  return entity_ui_get_form('group', $subgroup, 'create', $form_state);
}

/**
 * Implements hook_form_BASE_FORM_ID_alter().
 */
function ggroup_form_group_form_alter($form, &$form_state) {
  $subgroup = $form_state['group'];

  // If the group is a subgroup, add some magic.
  if (!empty($subgroup->group)) {
    $group = group_load($subgroup->group);
    $group_type = group_type_load($subgroup->type);

    $message = 'Create @group_type in @group';
    $replace = array(
      '@group_type' => $group_type->label(),
      '@group' => $group->label(),
    );

    drupal_set_title(t($message, $replace));
  }
}
