<?php
/**
 * @file
 * Entity API related helper functions for group member profiles.
 */

/**
 * Load multiple group member profiles.
 *
 * @param array $pids
 *   (optional) An array of GProfile ids to load.
 * @param bool $reset
 *   (optional) Whether to reset the internal cache.
 *
 * @return array
 *   An array of GProfile entities, keyed by their profile ids.
 *
 * @see entity_load()
 */
function gprofile_load_multiple(array $pids = array(), $reset = FALSE) {
  return entity_load('gprofile', $pids, array(), $reset);
}

/**
 * Load a group member profile.
 *
 * @param int $gid
 *   The group id to load the profile for.
 * @param int $uid
 *   The user id to load the profile for.
 * @param string $type
 *   The profile type to load.
 *
 * @return GProfile
 *   The loaded GProfile entity.
 */
function gprofile_load($gid, $uid, $type) {
  if ($group_membership = group_membership_load($gid, $uid)) {
    $profiles = gprofile_load_by_member($group_membership->mid, $type);
    return reset($profiles);
  }
}

/**
 * Load all group member profiles for a given membership.
 *
 * @param int $mid
 *   The GroupMembership id to load the profiles for.
 * @param string $type
 *   (optional) The GProfileType name to filter on.
 *
 * @return array
 *   An array of GProfile entities, keyed by their profile ids.
 */
function gprofile_load_by_member($mid, $type = NULL) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'gprofile');
  $query->propertyCondition('mid', $mid);

  if (!empty($type)) {
    $query->propertyCondition('type', $type);
  }

  $result = $query->execute();

  return isset($result['gprofile'])
    ? gprofile_load_multiple(array_keys($result['gprofile']))
    : array();
}

/**
 * Load all group member profiles for a given type.
 *
 * @param string $type
 *   The GProfileType to load the profiles for.
 *
 * @return array
 *   An array of GProfile entities, keyed by their profile ids.
 */
function gprofile_load_by_type($type) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'gprofile');
  $query->propertyCondition('type', $type);
  $result = $query->execute();

  return isset($result['gprofile'])
    ? gprofile_load_multiple(array_keys($result['gprofile']))
    : array();
}

/**
 * Delete multiple group member profiles.
 *
 * @param array $pids
 *   The ids of the profiles you want to delete.
 */
function gprofile_delete_multiple(array $pids) {
  entity_delete_multiple('gprofile', $pids);
}
