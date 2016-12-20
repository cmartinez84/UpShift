<?php
/**
 * @file
 * Group member invite callback and pages.
 */

/**
 * Generate the group invite page for a given user.
 *
 * @param object $account
 *   The user to display data for.
 *
 * @return array
 *   The render array for this page.
 *
 * @see ginvite_accept_callback()
 * @see ginvite_decline_callback()
 */
function ginvite_user_page($account) {
  // Create the invitations table.
  $page['invites'] = array(
    '#theme' => 'table',
    '#header' => array(
      'group' => t('Group name'),
      'roles' => t('Roles'),
      'operations' => t('Operations'),
    ),
    '#rows' => array(),
    '#empty' => t('You do not have any pending group invitations.'),
  );

  // Retrieve all group invitations (memberships).
  if ($invites = GroupMembership::getByStatus($account->uid, 'invited')) {
    // Link the rows variable to the table element's rows.
    $rows = &$page['invites']['#rows'];

    foreach ($invites as $group_membership) {
      $wrapper = entity_metadata_wrapper('group_membership', $group_membership);

      // Extract the invitation roles.
      $group_roles = array();
      foreach ($wrapper->roles->getIterator() as $group_role_wrapper) {
        $group_roles[] = $group_role_wrapper->label();
      }

      // Populate the invitation operations.
      $operations['accept'] = array(
        'title' => t('accept'),
        'href' => "user/$account->uid/group/invite/$group_membership->gid/accept",
      );

      $operations['decline'] = array(
        'title' => t('decline'),
        'href' => "user/$account->uid/group/invite/$group_membership->gid/decline",
      );

      $operations['view group'] = array(
        'title' => t('view group'),
        'href' => "group/$group_membership->gid",
      );

      $rows[] = array(
        'group' => $wrapper->group->label(),
        'roles' => array(
          'data' => array(
            '#theme' => 'item_list__group_roles',
            '#items' => $group_roles,
          ),
        ),
        'operations' => array(
          'data' => array(
            '#theme' => 'links__group_invitation_operation_links',
            '#links' => $operations,
            '#attributes' => array('class' => array('links', 'inline')),
          ),
        ),
      );
    }
  }

  return $page;
}

/**
 * Act upon accepting a group invitation from the user page.
 *
 * @see ginvite_user_page()
 */
function ginvite_accept_callback($account, Group $group) {
  $group_membership = $group->getMember($account->uid);
  $group_membership->status = 'active';
  $group_membership->save();

  drupal_set_message(t('You have accepted the group invitation.'));
  drupal_goto("user/$account->uid/group/invite");
}

/**
 * Act upon declining a group invitation from the user page.
 *
 * @see ginvite_user_page()
 */
function ginvite_decline_callback($account, Group $group) {
  $group_membership = $group->getMember($account->uid);
  $group_membership->delete();

  drupal_set_message(t('You have declined the group invitation.'));
  drupal_goto("user/$account->uid/group/invite");
}
