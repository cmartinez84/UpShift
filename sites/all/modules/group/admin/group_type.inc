<?php
/**
 * @file
 * Group type editing UI.
 */

/**
 * Generates the group type editing form.
 */
function group_type_form($form, &$form_state, GroupType $group_type, $op = 'edit') {
  $locked = entity_has_status('group_type', $group_type, ENTITY_IN_CODE);

  if ($op == 'clone') {
    $group_type->label .= ' (cloned)';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $group_type->label,
    '#description' => t('The human-readable name of this group type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $group_type->name,
    '#maxlength' => 32,
    '#disabled' => $locked || $op == 'edit',
    '#machine_name' => array(
      'exists' => 'group_type_load',
      'source' => array('label'),
      'label' => t('Machine name'),
      'replace_pattern' => '[^a-z0-9_]+',
      'replace' => '_',
    ),
    '#element_validate' => array('form_validate_machine_name', 'entity_ui_validate_machine_name'),
    '#description' => t('A unique machine-readable name for this group type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group type'),
    '#weight' => 40,
  );

  if (!$locked && $op == 'edit') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete group type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('group_type_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function group_type_form_submit(&$form, &$form_state) {
  $group_type = entity_ui_form_submit_build_entity($form, $form_state);
  $group_type->save();

  $form_state['redirect'] = 'admin/group/type';
}

/**
 * Form API submit callback for the delete button.
 */
function group_type_form_submit_delete(&$form, &$form_state) {
  $group_type = $form_state['group_type']->name;

  $form_state['redirect'] = "admin/group/type/manage/$group_type/delete";
}
