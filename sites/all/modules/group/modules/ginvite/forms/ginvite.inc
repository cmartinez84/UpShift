<?php
/**
 * @file
 * Group member invite UI.
 *
 * This file contains hook_form_alter() variants even though it isn't always
 * included from the .module file. The reason for this is that the forms they
 * alter reside in this file and are loaded by hook_menu() anyway.
 */

/**
 * Base form for inviting users.
 *
 * @see group_forms()
 */
function ginvite_form($form, &$form_state, Group $group) {
  $group_roles = group_type_load($group->type)->getRoles(FALSE);
  foreach ($group_roles as &$group_role) {
    $group_role = $group_role->label();
  }

  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles'),
    '#description' => t('Select the roles the member will receive should they accept the invite.'),
    '#options' => $group_roles,
    '#default_value' => array(),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send invitation'),
  );

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Build the group invite by user form.
 *
 * @see ginvite_forms()
 */
function group_form_ginvite_by_user_form_alter(&$form, &$form_state) {
  $form['user'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Enter the username of the person you want to invite.'),
    '#autocomplete_path' => 'user/autocomplete',
    '#required' => TRUE,
    '#weight' => -1,
  );
}

/**
 * Validate handler for ginvite_by_user_form().
 *
 * @see group_form_ginvite_by_user_form_alter()
 */
function ginvite_by_user_form_validate($form, &$form_state) {
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
 * Submit handler for ginvite_by_user_form().
 *
 * @see group_form_ginvite_by_user_form_alter()
 */
function ginvite_by_user_form_submit($form, &$form_state) {
  global $user;

  $values = array(
    'roles' => array_filter($form_state['values']['roles']),
    'status' => 'invited',
    'invited_on' => REQUEST_TIME,
    'invited_by' => $user->uid,
  );

  $form_state['build_info']['args'][0]->addMember($form_state['values']['uid'], $values);
  drupal_set_message(t('Invite sent to %user', array('%user' => $form_state['values']['user'])));
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Build the group invite by mail form.
 *
 * @see ginvite_forms()
 */
function group_form_ginvite_by_mail_form_alter(&$form, &$form_state) {
  $form['mail'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#description' => t('Enter the e-mail address of the person you want to invite.'),
    '#required' => TRUE,
    '#weight' => -1,
  );
}

/**
 * Validate handler for ginvite_by_mail_form().
 *
 * @see group_form_ginvite_by_mail_form_alter()
 */
function ginvite_by_mail_form_validate($form, &$form_state) {
  $group = $form_state['build_info']['args'][0];
  $mail = $form_state['values']['mail'];

  if (!valid_email_address($mail)) {
    form_set_error('mail', t('Invalid e-mail address.'));
  }

  // Validation if the e-mail address is already registered.
  if ($account = user_load_by_mail($mail)) {
    // First check for invite by user access.
    if (!group_access('invite members by user', $group)) {
      $message  = '%email is already registered on the website. ';
      $message .= 'You do not, however, have the permission to invite existing users.';
      form_set_error('mail', t($message, array('%email' => $mail)));
    }

    // Then check if the membership already exists.
    elseif ($group->getMember($account->uid)) {
      $message = '%email is already registered to a member of this group.';
      form_set_error('mail', t($message, array('%email' => $mail)));
    }

    // If nothing fails, set the required values for an invite by user.
    else {
      $form_state['values']['uid'] = $account->uid;
      $form_state['values']['user'] = format_username($account);
    }
  }

  // Validation if the e-mail address is not already registered.
  else {
    // Get all invites for the e-mail address.
    $invites = ginvite_get_invites($mail);

    // Check for an existing invite for this group.
    if (!empty($invites[$group->gid])) {
      $message = 'There is already an invite pending for %email.';
      form_set_error('mail', t($message, array('%email' => $mail)));
    }
  }
}

/**
 * Submit handler for ginvite_by_mail_form().
 *
 * @see group_form_ginvite_by_mail_form_alter()
 */
function ginvite_by_mail_form_submit($form, &$form_state) {
  $mail = $form_state['values']['mail'];

  // If the e-mail was already registered, send a user invite instead.
  if (!empty($form_state['values']['uid'])) {
    $message = 'There already is a user account for %email. Sending a regular invite instead.';
    drupal_set_message(t($message, array('%email' => $mail)), 'warning');

    ginvite_by_user_form_submit($form, $form_state);
  }

  // Otherwise, register the group invite in the database.
  else {
    global $user, $language;
    $group = $form_state['build_info']['args'][0];

    // Insert the invite record into the database.
    db_insert('group_invite')
      ->fields(array(
        'gid' => $form_state['build_info']['args'][0]->gid,
        'roles' => serialize(array_values(array_filter($form_state['values']['roles']))),
        'mail' => $mail,
        'invited_on' => REQUEST_TIME,
        'invited_by' => $user->uid,
      ))
      ->execute();

    // Actually e-mail the invite to the person.
    $params = array('user' => $user, 'group' => $group);
    drupal_mail('ginvite', 'invite', $mail, $language, $params);

    // Show a confirmation message.
    drupal_set_message(t('Invite sent to %mail', array('%mail' => $mail)));
  }
}
