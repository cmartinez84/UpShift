<?php
/**
 * @file
 * Group type configuration admin UI.
 */

/**
 * Configure the default roles a group creator should get.
 *
 * @see GroupController::save()
 */
function group_type_config_form($form, &$form_state, GroupType $group_type) {
  // Load the saved configuration.
  $defaults = isset($group_type->config['creator_roles'])
    ? drupal_map_assoc($group_type->config['creator_roles'])
    : array();

  // Get the available group roles for this group type.
  $group_roles = $group_type->getRoles(FALSE);

  // Convert the roles into valid checkbox options.
  foreach ($group_roles as &$group_role) {
    $group_role = $group_role->label();
  }

  // Add a checkbox for each available group role.
  $form['creator_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Creator roles'),
    '#description' => t('Configure the roles that should be assigned to a user when they create a group.'),
    '#options' => $group_roles,
    '#default_value' => $defaults,
  );

  // Show some help text when no roles are available.
  if (empty($group_roles)) {
    $message = t('There are no roles available for this group type.');
    $form['creator_roles']['#description'] = $message;
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for group_type_config_form().
 */
function group_type_config_form_submit($form, &$form_state) {
  $group_type = $form_state['build_info']['args'][0];
  $group_type->config['creator_roles'] = array_values(array_filter($form_state['values']['creator_roles']));
  $group_type->save();
}
