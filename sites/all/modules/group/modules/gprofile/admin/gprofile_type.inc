<?php
/**
 * @file
 * Group member profile type editing UI.
 */

/**
 * Generates the group member profile type editing form.
 */
function gprofile_type_form($form, &$form_state, GProfileType $gprofile_type, $op = 'edit') {
  $locked = entity_has_status('gprofile_type', $gprofile_type, ENTITY_IN_CODE);

  if ($op == 'clone') {
    $gprofile_type->label .= ' (cloned)';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $gprofile_type->label,
    '#description' => t('The human-readable name of this profile type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $gprofile_type->name,
    '#maxlength' => 32,
    '#disabled' => $locked || $op == 'edit',
    '#machine_name' => array(
      'exists' => 'gprofile_type_exists',
      'source' => array('label'),
      'label' => t('Machine name'),
      'replace_pattern' => '[^a-z0-9_]+',
      'replace' => '_',
    ),
    '#element_validate' => array('form_validate_machine_name', 'entity_ui_validate_machine_name'),
    '#description' => t('A unique machine-readable name for this profile type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save profile type'),
    '#weight' => 40,
  );

  if (!$locked && $op == 'edit') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete profile type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('gprofile_type_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function gprofile_type_form_submit(&$form, &$form_state) {
  $gprofile_type = entity_ui_form_submit_build_entity($form, $form_state);
  $gprofile_type->save();

  $form_state['redirect'] = 'admin/group/member';
}

/**
 * Form API submit callback for the delete button.
 */
function gprofile_type_form_submit_delete(&$form, &$form_state) {
  $gprofile_type = $form_state['gprofile_type']->name;

  $form_state['redirect'] = "admin/group/type/manage/$gprofile_type/delete";
}
