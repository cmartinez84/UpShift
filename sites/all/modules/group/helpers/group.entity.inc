<?php
/**
 * @file
 * Entity API related helper functions for groups.
 */

/**
 * Load multiple groups.
 *
 * @param array $gids
 *   (optional) An array of group ids to load.
 * @param bool $reset
 *   (optional) Whether to reset the internal cache.
 *
 * @return array
 *   An array of Group entities, keyed by their group ids.
 *
 * @see entity_load()
 */
function group_load_multiple(array $gids = array(), $reset = FALSE) {
  return entity_load('group', $gids, array(), $reset);
}

/**
 * Load a group.
 *
 * @param int $gid
 *   The group id of the group you want to load.
 * @param bool $reset
 *   (optional) Whether to reset the internal cache.
 *
 * @return Group
 *   The loaded Group entity.
 */
function group_load($gid, $reset = FALSE) {
  $groups = group_load_multiple(array($gid), array(), $reset);
  return reset($groups);
}

/**
 * Load all groups with a given property value.
 *
 * @param string $property
 *   The property name you wish to filter on.
 * @param string $value
 *   The property value you wish to filter on.
 *
 * @return array
 *   An array of Group entities, keyed by their group ids.
 */
function group_load_by_property($property, $value) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'group');
  $query->propertyCondition($property, $value);
  $result = $query->execute();

  return isset($result['group'])
    ? group_load_multiple(array_keys($result['group']))
    : array();
}

/**
 * Load all groups with a given name.
 *
 * @param string $name
 *   The name of the group you want to load.
 *
 * @return array
 *   An array of Group entities, keyed by their group ids.
 */
function group_load_by_name($name) {
  return group_load_by_property('title', $name);
}

/**
 * Load all groups of a given type.
 *
 * @param string $type
 *   The group type you want to load groups for.
 *
 * @return array
 *   An array of Group entities, keyed by their group ids.
 */
function group_load_by_type($type) {
  return group_load_by_property('type', $type);
}

/**
 * Load all groups for a given user.
 *
 * @param int $uid
 *   The id of the user.
 * @param string $type
 *   (optional) Filters the results by a group type.
 *
 * @return array
 *   An array of Group entities, keyed by their group ids.
 */
function group_load_by_member($uid, $type = NULL) {
  $gids = array();
  foreach (group_membership_load_by_user($uid) as $group_membership) {
    $gids[] = $group_membership->gid;
  }

  $groups = group_load_multiple($gids);
  if ($type) {
    foreach ($groups as $gid => $group) {
      if ($group->type !== $type) {
        unset($groups[$gid]);
      }
    }
  }

  return $groups;
}

/**
 * Delete multiple groups.
 *
 * @param array $gids
 *   The ids of the groups you want to delete.
 */
function group_delete_multiple(array $gids) {
  entity_delete_multiple('group', $gids);
}
