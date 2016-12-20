<?php
/**
 * @file
 * Group membership editing UI.
 */

/**
 * Generates the group membership editing form.
 */
function group_membership_form($form, &$form_state, GroupMembership $group_membership, $op = 'edit') {
  $form['#entity_builders'][] = 'group_membership_restore_roles_eb';

  $group = group_load($group_membership->gid);
  $account = user_load($group_membership->uid);

  if ($op == 'edit') {
    $message = 'Edit membership for %user in %group';
    $replace = array(
      '%user' => format_username($account),
      '%group' => $group->label(),
    );
    drupal_set_title(t($message, $replace), PASS_THROUGH);
  }

  // Only show the role selector if the current user can administer members.
  if (group_access('administer members', $group)) {
    $roles = array();
    foreach ($group->getRoles(FALSE) as $group_role) {
      $roles[$group_role->name] = $group_role->label();
    }

    $form['roles'] = array(
      '#title' => t('Group roles'),
      '#type' => 'checkboxes',
      '#options' => $roles,
      '#default_value' => $group_membership->roles,
    );
  }

  // Load fields based on group type.
  field_attach_form('group_membership', $group_membership, $form, $form_state, entity_language('group_membership', $group_membership));

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit membership'),
    '#weight' => 40,
  );

  return $form;
}

/**
 * Form API validate callback for the group membership form.
 */
function group_membership_form_validate($form, &$form_state) {
  entity_form_field_validate('group_membership', $form, $form_state);
}

/**
 * Group membership edit submit callback.
 */
function group_membership_form_submit($form, &$form_state) {
  $group_membership = entity_ui_form_submit_build_entity($form, $form_state);

  // If roles were edited, change and save them.
  if (!empty($form_state['values']['roles'])) {
    $group_membership->changeRoles($form_state['values']['roles']);
  }
  // Otherwise just save the membership.
  else {
    $group_membership->save();
  }

  drupal_set_message(t('The membership was successfully edited.'));
}

/**
 * Entity builder: Restore original roles to the GroupMembership.
 *
 * Because entity_form_submit_build_entity() sets all non-field values as
 * entity properties, GroupMembership::changeRoles() may get confused. To
 * rectify this, we reset the roles to the original values.
 *
 * @see group_membership_form_submit()
 * @see entity_form_submit_build_entity()
 */
function group_membership_restore_roles_eb($entity_type, $entity, $form, $form_state) {
  if ($entity_type == 'group_membership' && !empty($entity->mid)) {
    $original = entity_load_unchanged($entity_type, $entity->mid);
    $entity->roles = $original->roles;
  }
}

/**
 * Form builder for the "Join group" and "Request membership" forms.
 *
 * @see GroupMembership::hook_forms()
 */
function group_membership_join_form_builder(Group $group, $account, $form_id) {
  // Build an empty GroupMembership.
  $values = array('gid' => $group->gid, 'uid' => $account->uid);
  $group_membership = entity_create('group_membership', $values);

  // Create our own form state and build the form.
  $form_state['entity_type'] = 'group_membership';
  $form_state['group_membership'] = $group_membership;
  $form_state['redirect'] = "group/$group->gid";
  $form_state['build_info']['args'] = array($group_membership, 'add', 'group_membership');

  return drupal_build_form($form_id, $form_state);
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function group_form_group_membership_join_form_alter(&$form, &$form_state) {
  $form['actions']['submit']['#value'] = t('Join');
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function group_form_group_membership_request_form_alter(&$form, &$form_state) {
  $form['actions']['submit']['#value'] = t('Request membership');

  // See group_membership_request_form_submit_b() for the naming reason.
  array_unshift($form['#submit'], 'group_membership_request_form_submit_b');
}

/**
 * Submit handler: Sets the membership status to 'requested'.
 *
 * Intentionally not named group_membership_request_form_submit to make sure
 * the submit handler of the base form (group_membership_form_submit) gets
 * automatically set.
 *
 * This way, we can make sure this submit handler fires first and only then the
 * base form's submit handler. Otherwise, any module altering the base form
 * would get first say in altering #submit.
 *
 * @see drupal_prepare_form()
 */
function group_membership_request_form_submit_b($form, &$form_state) {
  $form_state['group_membership']->status = 'requested';
  $form_state['group_membership']->requested_on = REQUEST_TIME;
}
