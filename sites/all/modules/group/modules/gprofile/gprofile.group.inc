<?php
/**
 * @file
 * Hook implementations for the Group module.
 */

/**
 * Implements hook_group_permission().
 */
function gprofile_group_permission() {
  $permissions = array();

  foreach (gprofile_types() as $name => $gprofile_type) {
    $replace = array('%profile' => $gprofile_type->label());

    $permissions["use $name profile"] = array(
      'title' => t('Use %profile profile', $replace),
      'description' => t('Fill out %profile profile information', $replace),
      'limit to' => GROUP_LIMIT_MEMBER,
    );
  }

  return $permissions;
}

/**
 * Implements hook_group_membership_profile_info().
 */
function gprofile_group_membership_profile_info() {
  $profiles = array();

  foreach (gprofile_types() as $name => $gprofile_type) {
    $profiles[$name] = array(
      'label' => $gprofile_type->label(),
      'entity status' => $gprofile_type->status,
      'path' => "admin/group/member/manage/$name",
    );
  }

  return $profiles;
}
