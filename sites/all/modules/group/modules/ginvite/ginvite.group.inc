<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 */
function ginvite_group_permission() {
  $permissions = array(
    'invite members by user' => array(
      'title' => t('Invite members by user'),
      'description' => t('Invite members by their username on the website'),
      'warning' => t('You only get the autocomplete if you have the sitewide "View user profiles" permission.'),
    ),
    'invite members by mail' => array(
      'title' => t('Invite members by mail'),
      'description' => t('Invite members by their e-mail address'),
      'warning' => t('Registered e-mail addresses will get a user invite instead.'),
    ),
    'manage mail invitations' => array(
      'title' => t('Manage e-mail invitations'),
      'description' => t('View, edit or delete unused e-mail invitations'),
    ),
  );

  return $permissions;
}

/**
 * Implements hook_group_membership_metadata().
 */
function ginvite_group_membership_metadata(GroupMembership $group_membership) {
  $metadata = array();

  // Create an EMW to have easy access to the data we need.
  $emw = entity_metadata_wrapper('group_membership', $group_membership);

  if (!empty($group_membership->invited_on)) {
    $metadata[] = array(
      'description' => t('Invited on'),
      'value' => format_date($emw->invited_on->value()),
    );

    $metadata[] = array(
      'description' => t('Invited by'),
      'value' => l($emw->invited_by->label(), 'user/' . $group_membership->invited_by),
    );
  }

  return $metadata;
}

/**
 * Implements hook_group_membership_status_info().
 */
function ginvite_group_membership_status_info() {
  $info['invited'] = array(
    'title' => t('Invited'),
    'active' => FALSE,
  );

  return $info;
}

/**
 * Implements hook_group_membership_action_info().
 */
function ginvite_group_membership_action_info() {
  $info['ginvite_accept'] = array(
    'label' => t('Accept'),
    'description' => t('You have been invited to join this group'),
    'access callback' => 'ginvite_user_is_invited',
    'action callback' => 'ginvite_user_accept',
  );

  $info['ginvite_decline'] = array(
    'label' => t('Decline'),
    'description' => t('You have been invited to join this group'),
    'access callback' => 'ginvite_user_is_invited',
    'action callback' => 'ginvite_user_decline',
  );

  return $info;
}

/**
 * Check whether a user has a pending group invitation.
 *
 * @param Group $group
 *   The group the invitation is for.
 * @param object $account
 *   The user the invitation is for.
 * @param GroupMembership|false $group_membership
 *   The group membership object to check for the invitation.
 *
 * @return bool
 *   Whether the user has an invitation.
 */
function ginvite_user_is_invited(Group $group, $account, $group_membership) {
  return !empty($group_membership) && $group_membership->status == 'invited';
}

/**
 * Accept a pending group invitation.
 *
 * @param Group $group
 *   The group the invitation is for.
 * @param object $account
 *   The user the invitation is for.
 * @param GroupMembership|false $group_membership
 *   The group membership object for the invitation.
 */
function ginvite_user_accept(Group $group, $account, $group_membership) {
  $group_membership->status = 'active';
  $group_membership->save();
  drupal_set_message(t('You have accepted the group invitation.'));
}

/**
 * Accept a pending group invitation.
 *
 * @param Group $group
 *   The group the invitation is for.
 * @param object $account
 *   The user the invitation is for.
 * @param GroupMembership|false $group_membership
 *   The group membership object for the invitation.
 */
function ginvite_user_decline(Group $group, $account, $group_membership) {
  $group_membership->delete();
  drupal_set_message(t('You have declined the group invitation.'));
}
