<?php
/**
 * @file
 * Contains all router functions for the Group module.
 */

/**
 * Implements hook_menu().
 *
 * @see GroupUiController::hook_menu()
 * @see GroupRoleUiController::hook_menu()
 * @see GroupTypeUiController::hook_menu()
 * @see GroupMembershipUiController::hook_menu()
 */
function group_menu() {
  $items['user/%user/group'] = array(
    'title' => 'Group',
    'description' => 'Group info for a given user.',
    'page callback' => 'group_user_page',
    'page arguments' => array(1),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/group.user.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['user/%user/group/list'] = array(
    'title' => 'My groups',
    'title callback' => 'group_user_page_title',
    'title arguments' => array(1),
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );

  return $items;
}

/**
 * Implements hook_menu_local_tasks_alter().
 *
 * Adds a local task to admin/group.
 */
function group_menu_local_tasks_alter(&$data, $router_item, $root_path) {
  // Add action link to 'group/add' on 'admin/group' page.
  if ($root_path == 'admin/group') {
    $item = menu_get_item('group/add');

    if ($item['access']) {
      $data['actions']['output'][] = array(
        '#theme' => 'menu_local_action',
        '#link' => $item,
      );
    }
  }
}

/**
 * Access callback: Checks whether the user has permission to add a group.
 *
 * @see GroupUiController::hook_menu()
 */
function _group_add_access() {
  foreach (group_types() as $group_type) {
    if (group_entity_access('create', NULL, NULL, 'group', $group_type->name)) {
      return TRUE;
    }
  }

  if (user_access('configure group module')) {
    // There are no group types defined that the user has permission to create,
    // but the user does have the permission to administer the group types, so
    // grant them access to the page anyway.
    return TRUE;
  }

  return FALSE;
}

/**
 * Access callback: Checks if a role can be added to a group type.
 *
 * @see GroupTypeUiController::hook_menu()
 */
function group_type_add_role_access(GroupType $group_type) {
  return user_access('configure group module') && !entity_has_status('group_type', $group_type, ENTITY_FIXED);
}

/**
 * Access callback: Whether the user can join a group through a form.
 *
 * @see GroupMembershipUiController::hook_menu()
 */
function group_membership_join_form_access(Group $group, $account) {
  module_load_include('inc', 'group', 'group.group');
  return group_user_can_join_form($group, $account, $group->getMember($account->uid));
}

/**
 * Access callback: Whether the user can request a membership through a form.
 *
 * @see GroupMembershipUiController::hook_menu()
 */
function group_membership_request_form_access(Group $group, $account) {
  module_load_include('inc', 'group', 'group.group');
  return group_user_can_request_membership_form($group, $account, $group->getMember($account->uid));
}

/**
 * Title callback: Returns the group title.
 *
 * @see GroupUiController::hook_menu()
 */
function group_page_title(Group $group) {
  return $group->label();
}

/**
 * Title callback: Returns the group membership title.
 *
 * @see GroupMembershipUiController::hook_menu()
 */
function group_membership_page_title(GroupMembership $group_membership) {
  return strip_tags($group_membership->label());
}

/**
 * Title callback for user/%user/group/list.
 */
function group_user_page_title($account) {
  global $user;

  if ($user->uid == $account->uid) {
    return t('My groups');
  }

  return t("@user's groups", array('@user' => format_username($account)));
}

/**
 * Title callback for the 'join group' form.
 *
 * @see GroupMembershipUiController::hook_menu()
 */
function group_membership_join_title(Group $group) {
  return t('Join @group', array('@group' => $group->label()));
}

/**
 * Title callback for the 'request membership' form.
 *
 * @see GroupMembershipUiController::hook_menu()
 */
function group_membership_request_title(Group $group) {
  return t('Request membership for @group', array('@group' => $group->label()));
}
