<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 */
function gadd_group_permission() {
  $permissions = array(
    'add member' => array(
      'title' => t('Add members'),
      'description' => t('Forcibly add members by their username on the website'),
      'warning' => t('You only get the autocomplete if you have the sitewide "View user profiles" permission.'),
    ),
  );

  return $permissions;
}

/**
 * Implements hook_group_membership_metadata().
 */
function gadd_group_membership_metadata(GroupMembership $group_membership) {
  $metadata = array();

  // Create an EMW to have easy access to the data we need.
  $emw = entity_metadata_wrapper('group_membership', $group_membership);

  if (!empty($group_membership->added_on)) {
    $metadata[] = array(
      'description' => t('Added on'),
      'value' => format_date($emw->added_on->value()),
    );

    $metadata[] = array(
      'description' => t('Added by'),
      'value' => l($emw->added_by->label(), 'user/' . $group_membership->added_by),
    );
  }

  return $metadata;
}
