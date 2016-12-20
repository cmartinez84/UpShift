<?php
/**
 * @file
 * Group role editing UI.
 */

/**
 * Wrapper around group_role_form().
 *
 * Called from hook_menu() to dynamically prepopulate the
 * group type the newly created group role belongs to.
 *
 * @see GroupTypeUIController::hook_menu()
 */
function group_type_add_role_form(GroupType $group_type) {
  $group_role = entity_create('group_role', array('type' => $group_type->name, 'global' => 0));
  return entity_ui_get_form('group_role', $group_role, 'add');
}

/**
 * Generates the group role editing form.
 */
function group_role_form($form, &$form_state, GroupRole $group_role, $op = 'edit') {
  $locked = entity_has_status('group_role', $group_role, ENTITY_IN_CODE);

  if ($op == 'clone') {
    $group_role->label .= ' (cloned)';
  }

  if (empty($group_role->global) && !empty($group_role->type)) {
    $form['type'] = array(
      '#type' => 'value',
      '#value' => $group_role->type,
    );
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $group_role->label,
    '#description' => t('The human-readable name of this group role.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $group_role->name,
    '#maxlength' => 32,
    '#disabled' => $locked || $op == 'edit',
    '#machine_name' => array(
      'exists' => 'group_role_exists',
      'source' => array('label'),
      'label' => t('Machine name'),
      'replace_pattern' => '[^a-z0-9_]+',
      'replace' => '_',
    ),
    '#element_validate' => array('form_validate_machine_name', 'entity_ui_validate_machine_name'),
    '#description' => t('A unique machine-readable name for this group role. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group role'),
    '#weight' => 40,
  );

  if (!$locked && $op == 'edit') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete group role'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('group_role_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function group_role_form_submit(&$form, &$form_state) {
  $group_role = entity_ui_form_submit_build_entity($form, $form_state);
  $group_role->save();

  // Redirect to the global roles or group type roles page, depending on
  // whether there was a group type specified.
  $form_state['redirect'] = !empty($form_state['values']['type'])
    ? 'admin/group/type/manage/' . $form_state['values']['type'] . '/permissions/roles'
    : 'admin/group/role';
}

/**
 * Form API submit callback for the delete button.
 */
function group_role_form_submit_delete(&$form, &$form_state) {
  $group_role = $form_state['group_role']->name;

  $form_state['redirect'] = "admin/group/role/manage/$group_role/delete";
}
