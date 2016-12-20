<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 */
function group_group_permission() {
  $combo_warning = '<br />' . t('Enabling multiple means of joining a group may result in a weird user experience');

  $permissions = array(
    'administer group' => array(
      'title' => t('Administer group'),
      'description' => t('Administer the group, its content and members'),
      'restrict access' => TRUE,
    ),
    'administer members' => array(
      'title' => t('Administer group members'),
      'description' => t('Administer the group members'),
      'restrict access' => TRUE,
    ),
    'access member overview' => array(
      'title' => t('Access the member overview page'),
    ),
    'access member pages' => array(
      'title' => t('View the individual member pages'),
    ),
    'access member metadata' => array(
      'title' => t('View the individual member metadata: Join date, invited by, ...'),
    ),
    'edit own membership' => array(
      'title' => t('Edit own membership'),
      'description' => t('Edit own membership information'),
      'limit to' => GROUP_LIMIT_MEMBER,
    ),
    'edit group' => array(
      'title' => t('Edit group'),
      'description' => t('Edit the group information'),
    ),
    'delete group' => array(
      'title' => t('Delete group'),
      'description' => t('Delete the group'),
    ),
    'join group' => array(
      'title' => t('Join group'),
      'description' => t('Join with the click of a button') . $combo_warning,
      'limit to' => GROUP_LIMIT_OUTSIDER,
    ),
    'join group form' => array(
      'title' => t('Join group (form)'),
      'description' => t('Join by filling out the member form') . $combo_warning,
      'limit to' => GROUP_LIMIT_OUTSIDER,
    ),
    'request membership' => array(
      'title' => t('Request group membership'),
      'description' => t('Request to join the group with the click of a button') . $combo_warning,
      'limit to' => GROUP_LIMIT_OUTSIDER,
    ),
    'request membership form' => array(
      'title' => t('Request group membership (form)'),
      'description' => t('Request to join the group by filling out a member form') . $combo_warning,
      'limit to' => GROUP_LIMIT_OUTSIDER,
    ),
    'leave group' => array(
      'title' => t('Leave group'),
      'limit to' => GROUP_LIMIT_MEMBER,
    ),
    'view group' => array(
      'title' => t('View group'),
    ),
    'view members' => array(
      'title' => t('View group members'),
    ),
  );

  return $permissions;
}

/**
 * Implements hook_group_filters().
 */
function group_group_filters() {
  // Get a list of all group types.
  $group_types = array();
  foreach (group_types() as $name => $group_type) {
    $group_types[$name] = $group_type->label();
  }

  // Build a group type filter.
  $filters['type'] = array(
    'title' => t('Group type'),
    'options' => array(
      '[any]' => t('any'),
    ) + $group_types,
  );

  return $filters;
}

/**
 * Implements hook_group_operations().
 */
function group_group_operations() {
  $operations['delete'] = array(
    'label' => t('Delete selected groups'),
    'callback' => 'group_multiple_delete_confirm',
    'form callback' => TRUE,
  );

  // Add Pathauto module support.
  if (module_exists('pathauto') && module_exists('entity_tokens')) {
    module_load_include('inc', 'group', 'group.pathauto');
    $operations['pathauto_update_alias'] = array(
      'label' => t('Update URL alias'),
      'callback' => 'group_pathauto_update_alias_multiple',
      'callback arguments' => array('bulkupdate', array('message' => TRUE)),
    );
  }

  return $operations;
}

/**
 * Implements hook_group_operation_links().
 */
function group_group_operation_links(Group $group) {
  $operations = array();

  // Retrieve the destination url.
  $destination = drupal_get_destination();

  // Add an 'edit' link if available.
  if (group_access('update', $group)) {
    $operations['edit'] = array(
      'title' => t('edit'),
      'href' => "group/$group->gid/edit",
      'query' => $destination,
    );
  }

  // Add a 'delete' link if available.
  if (group_access('delete', $group)) {
    $operations['delete'] = array(
      'title' => t('delete'),
      'href' => "group/$group->gid/delete",
      'query' => $destination,
    );
  }

  return $operations;
}

/**
 * Implements hook_group_member_filters().
 */
function group_group_member_filters() {
  // Build a status filter.
  $filters['status'] = array(
    'title' => t('Status'),
    'options' => array('[any]' => t('any')) + group_membership_status_types(),
  );

  return $filters;
}

/**
 * Implements hook_group_member_operations().
 *
 * Callbacks are situated in forms/group.members.inc.
 */
function group_group_member_operations(Group $group) {
  $operations['remove'] = array(
    'label' => t('Remove selected members'),
    'callback' => 'group_membership_multiple_delete_confirm',
    'form callback' => TRUE,
  );

  $operations['approve'] = array(
    'label' => t('Approve selected membership requests'),
    'callback' => 'group_membership_approve',
    'optgroup' => t('Approve or decline membership requests'),
  );

  $operations['decline'] = array(
    'label' => t('Decline selected membership requests'),
    'callback' => 'group_membership_decline',
    'optgroup' => t('Approve or decline membership requests'),
  );

  $operations['block'] = array(
    'label' => t('Block selected members'),
    'callback' => 'group_membership_block',
    'optgroup' => t('Block or unblock selected members'),
  );

  $operations['unblock'] = array(
    'label' => t('Unblock selected members'),
    'callback' => 'group_membership_unblock',
    'optgroup' => t('Block or unblock selected members'),
  );

  foreach (group_type_load($group->type)->getRoles(FALSE) as $group_role) {
    $operations["add-$group_role->name"] = array(
      'label' => $group_role->label(),
      'callback' => 'group_membership_add_role',
      'callback arguments' => array($group_role->name),
      'optgroup' => t('Add role to selected members'),
    );

    $operations["remove-$group_role->name"] = array(
      'label' => $group_role->label(),
      'callback' => 'group_membership_remove_role',
      'callback arguments' => array($group_role->name),
      'optgroup' => t('Remove role from selected members'),
    );
  }

  return $operations;
}

/**
 * Implements hook_group_member_operation_links().
 */
function group_group_member_operation_links(GroupMembership $group_membership) {
  $operations = array();

  // Retrieve the destination url.
  $destination = drupal_get_destination();

  // Load the group the member belongs to.
  $group = group_load($group_membership->gid);

  // Add membership management links.
  if (group_access('administer members', $group)) {
    $operations['edit-membership'] = array(
      'title' => t('edit'),
      'href' => 'group/' . $group->gid . '/member/' . $group_membership->uid . '/edit',
      'query' => $destination,
    );

    $operations['cancel-membership'] = array(
      'title' => t('cancel'),
      'href' => 'group/' . $group->gid . '/member/' . $group_membership->uid . '/cancel',
      'query' => $destination,
    );
  }

  // Add the membership metadata link.
  if (group_access('access member metadata', $group)) {
    $operations['membership-metadata'] = array(
      'title' => t('metadata'),
      'href' => 'group/' . $group->gid . '/member/' . $group_membership->uid . '/metadata',
    );
  }

  return $operations;
}

/**
 * Implements hook_group_membership_metadata().
 */
function group_group_membership_metadata(GroupMembership $group_membership) {
  $metadata = array();

  // Create an EMW to have easy access to the data we need.
  $emw = entity_metadata_wrapper('group_membership', $group_membership);

  if (!empty($group_membership->joined_on)) {
    $metadata[] = array(
      'description' => t('Joined on'),
      'value' => format_date($emw->joined_on->value()),
    );
  }

  if (!empty($group_membership->requested_on)) {
    $metadata[] = array(
      'description' => t('Requested membership on'),
      'value' => format_date($emw->requested_on->value()),
    );
  }

  if (!empty($group_membership->approved_on)) {
    $metadata[] = array(
      'description' => t('Approved on'),
      'value' => format_date($emw->approved_on->value()),
    );

    $metadata[] = array(
      'description' => t('Approved by'),
      'value' => l($emw->approved_by->label(), 'user/' . $group_membership->approved_by),
    );
  }

  if (!empty($group_membership->blocked_on)) {
    $metadata[] = array(
      'description' => t('Blocked on'),
      'value' => format_date($emw->blocked_on->value()),
    );

    $metadata[] = array(
      'description' => t('Blocked by'),
      'value' => l($emw->blocked_by->label(), 'user/' . $group_membership->blocked_by),
    );

    $metadata[] = array(
      'description' => t('Status before being blocked'),
      'value' => $emw->blocked_status->label(),
    );
  }

  return $metadata;
}

/**
 * Implements hook_group_membership_status_info().
 */
function group_group_membership_status_info() {
  $info['active'] = array(
    'title' => t('Active'),
    'active' => TRUE,
  );

  $info['requested'] = array(
    'title' => t('Requested'),
    'active' => FALSE,
  );

  $info['blocked'] = array(
    'title' => t('Blocked'),
    'active' => FALSE,
  );

  return $info;
}

/**
 * Implements hook_group_membership_action_info().
 */
function group_group_membership_action_info() {
  $info['group_join'] = array(
    'label' => t('Join'),
    'description' => t('This group is free for anyone to join'),
    'access callback' => 'group_user_can_join',
    'action callback' => 'group_user_join',
  );

  $info['group_join_form'] = array(
    'label' => t('Join'),
    'description' => t('Fill out your details to join this group'),
    'access callback' => 'group_user_can_join_form',
    'action callback' => 'group_user_join_form',
  );

  $info['group_request_membership'] = array(
    'label' => t('Request membership'),
    'description' => t('This group requires admin approval to join'),
    'access callback' => 'group_user_can_request_membership',
    'action callback' => 'group_user_request_membership',
  );

  $info['group_request_membership_form'] = array(
    'label' => t('Request membership'),
    'description' => t('Fill out a membership request'),
    'access callback' => 'group_user_can_request_membership_form',
    'action callback' => 'group_user_request_membership_form',
  );

  $info['group_leave'] = array(
    'label' => t('Leave'),
    'description' => t('You are member of this group'),
    'access callback' => 'group_user_can_leave',
    'action callback' => 'group_user_leave',
  );

  return $info;
}

/**
 * Check whether a user can join a group.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership if one exists already.
 *
 * @return bool
 *   Whether the user can join the group.
 */
function group_user_can_join(Group $group, $account, $group_membership) {
  return empty($group_membership) && group_access('join group', $group, $account);
}

/**
 * Join a group.
 *
 * @param Group $group
 *   The group to join.
 * @param object $account
 *   The user that wants to join.
 */
function group_user_join(Group $group, $account) {
  $group->addMember($account->uid, array('joined_on' => REQUEST_TIME));
  drupal_set_message(t('You have joined the group.'));
}

/**
 * Check whether a user can join a group through a form.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership if one exists already.
 *
 * @return bool
 *   Whether the user can join the group.
 */
function group_user_can_join_form(Group $group, $account, $group_membership) {
  return empty($group_membership) && group_access('join group form', $group, $account);
}

/**
 * Display the member join form.
 *
 * @param Group $group
 *   The group to join.
 * @param object $account
 *   The user that wants to join.
 */
function group_user_join_form(Group $group, $account) {
  drupal_goto("group/$group->gid/member/$account->uid/join");
}

/**
 * Check whether a user can request to join a group.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership if one exists already.
 *
 * @return bool
 *   Whether the user can request a membership for the group.
 */
function group_user_can_request_membership(Group $group, $account, $group_membership) {
  return empty($group_membership) && group_access('request membership', $group, $account);
}

/**
 * Request to join a group.
 *
 * @param Group $group
 *   The group to join.
 * @param object $account
 *   The user that wants to join.
 */
function group_user_request_membership(Group $group, $account) {
  $group->addMember($account->uid, array('status' => 'requested', 'requested_on' => REQUEST_TIME));
  drupal_set_message(t('You have requested to join the group.'));
}

/**
 * Check whether a user can request to join a group through a form.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership if one exists already.
 *
 * @return bool
 *   Whether the user can request a membership for the group.
 */
function group_user_can_request_membership_form(Group $group, $account, $group_membership) {
  return empty($group_membership) && group_access('request membership form', $group, $account);
}

/**
 * Display the member request form.
 *
 * @param Group $group
 *   The group to join.
 * @param object $account
 *   The user that wants to join.
 */
function group_user_request_membership_form(Group $group, $account) {
  drupal_goto("group/$group->gid/member/$account->uid/request");
}

/**
 * Check whether a user can leave a group.
 *
 * @param Group $group
 *   The group to check access for.
 * @param object $account
 *   The user to check access for.
 * @param GroupMembership|false $group_membership
 *   The group membership of the user that wants to leave.
 *
 * @return bool
 *   Whether the user can leave the group.
 */
function group_user_can_leave(Group $group, $account, $group_membership) {
  return !empty($group_membership) && group_access('leave group', $group, $account);
}

/**
 * Leave a group.
 *
 * @param Group $group
 *   The group to leave.
 * @param object $account
 *   The user that wants to leave.
 * @param GroupMembership $group_membership
 *   The group membership of the user that wants to leave.
 */
function group_user_leave(Group $group, $account, GroupMembership $group_membership) {
  $group_membership->delete();
  drupal_set_message(t('You have left the group.'));
}
