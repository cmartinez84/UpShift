<?php
/**
 * @file
 * Entity API related helper functions for group memberships.
 */

/**
 * Load multiple group memberships by id.
 *
 * @param array $mids
 *   (optional) An array of group membership ids to load.
 * @param bool $reset
 *   (optional) Whether to reset the internal cache.
 *
 * @return array
 *   An array of GroupMembership entities, keyed by their membership ids.
 *
 * @see entity_load()
 */
function group_membership_load_multiple(array $mids = array(), $reset = FALSE) {
  return entity_load('group_membership', $mids, array(), $reset);
}

/**
 * Load all group memberships, optionally filtered by property values.
 *
 * @param array $filters
 *   (optional) An associative array with property names for keys and the
 *   values you wish to filter on as accompanying array values.
 *
 * @return array
 *   An array of GroupMembership entities, keyed by their membership ids.
 */
function group_memberships(array $filters = array()) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'group_membership');

  foreach ($filters as $property => $value) {
    $query->propertyCondition($property, $value);
  }
  $result = $query->execute();

  return isset($result['group_membership'])
    ? group_membership_load_multiple(array_keys($result['group_membership']))
    : array();
}

/**
 * Load a group membership by group and user id.
 *
 * @param int $gid
 *   The group id to load the membership for.
 * @param int $uid
 *   The user id to load the membership for.
 *
 * @return GroupMembership
 *   The loaded GroupMembership entity.
 */
function group_membership_load($gid, $uid) {
  $group_memberships = group_memberships(array('gid' => $gid, 'uid' => $uid));
  return reset($group_memberships);
}

/**
 * Load all group memberships for a given group.
 *
 * @param int $gid
 *   The group id to load the memberships for.
 *
 * @return array
 *   An array of GroupMembership entities, keyed by their membership ids.
 */
function group_membership_load_by_group($gid) {
  return group_memberships(array('gid' => $gid));
}

/**
 * Load all group memberships for a given user.
 *
 * @param int $uid
 *   The user id to load the memberships for.
 *
 * @return array
 *   An array of GroupMembership entities, keyed by their membership ids.
 */
function group_membership_load_by_user($uid) {
  return group_memberships(array('uid' => $uid));
}

/**
 * Delete multiple groups.
 */
function group_membership_delete_multiple($mids) {
  entity_delete_multiple('group_membership', $mids);
}
