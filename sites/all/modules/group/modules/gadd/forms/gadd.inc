<?php
/**
 * @file
 * Group add member UI.
 */

/**
 * Form for forcibly adding users.
 */
function gadd_add_member_form($form, &$form_state, Group $group) {
  $form['user'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Enter the username of the person you want to add.'),
    '#autocomplete_path' => 'user/autocomplete',
    '#required' => TRUE,
  );

  $group_roles = group_type_load($group->type)->getRoles(FALSE);
  foreach ($group_roles as &$group_role) {
    $group_role = $group_role->label();
  }

  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles'),
    '#description' => t('Select the roles the member will receive.'),
    '#options' => $group_roles,
    '#default_value' => array(),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add member'),
  );

  return $form;
}

/**
 * Validate handler for gadd_add_member_form().
 */
function gadd_add_member_form_validate($form, &$form_state) {
  $username = $form_state['values']['user'];

  // Check if the username exists.
  if (!($account = user_load_by_name($username))) {
    form_set_error('user', t('User %user not found', array('%user' => $username)));
  }

  else {
    // Save the uid for reusability's sake.
    $form_state['values']['uid'] = $account->uid;

    // Check if the membership already exists.
    if ($form_state['build_info']['args'][0]->getMember($account->uid)) {
      form_set_error('user', t('User %user is already a member of this group', array('%user' => $username)));
    }
  }
}

/**
 * Submit handler for gadd_add_member_form().
 */
function gadd_add_member_form_submit($form, &$form_state) {
  global $user;

  $values = array(
    'roles' => array_filter($form_state['values']['roles']),
    'added_on' => REQUEST_TIME,
    'added_by' => $user->uid,
  );

  $form_state['build_info']['args'][0]->addMember($form_state['values']['uid'], $values);
  drupal_set_message(t('Added %user', array('%user' => $form_state['values']['user'])));
}
