<?php
/**
 * @file
 * Entity API related helper functions for group roles.
 */

/**
 * Load group roles.
 *
 * @param array $names
 *   (optional) An array of machine names to filter on.
 *
 * @return array
 *   An array of GroupRole objects, keyed by their machine names.
 *
 * @see entity_load_multiple_by_name()
 */
function group_roles($names = FALSE) {
  return entity_load_multiple_by_name('group_role', $names);
}

/**
 * Retrieve all group role labels.
 *
 * Ideally used for populating option lists.
 *
 * @param bool $prefix_type
 *   (optional) Whether to prefix the group role label with a context label.
 *   This is 'Global' for global roles or the group type label otherwise.
 *
 * @return array
 *   An array of GroupRole labels, keyed by their machine names.
 */
function group_role_labels($prefix_type = FALSE) {
  $group_roles = group_roles();

  // Only load the group types if we want to prefix the group role labels.
  if ($prefix_type) {
    $group_types = group_type_labels();

    foreach ($group_roles as &$group_role) {
      $prefix = $group_role->global ? t('Global') : $group_types[$group_role->type];
      $group_role = "$prefix: " . $group_role->label();
    }
  }

  // Otherwise, stick to a simple loop.
  else {
    foreach ($group_roles as &$group_role) {
      $group_role = $group_role->label();
    }
  }

  return $group_roles;
}

/**
 * Load a group role by name.
 *
 * @param string $name
 *   The name of the group type you want to load.
 *
 * @return GroupRole
 *   The loaded GroupRole entity.
 */
function group_role_load($name) {
  $group_roles = group_roles(array($name));
  return reset($group_roles);
}

/**
 * Load multiple group roles by id.
 *
 * @param array $rids
 *   (optional) An array of group role ids to load.
 * @param bool $reset
 *   (optional) Whether to reset the internal cache.
 *
 * @return array
 *   An array of GroupRole entities, keyed by their machine names.
 *
 * @see entity_load()
 */
function group_role_load_multiple(array $rids = array(), $reset = FALSE) {
  return entity_load('group_role', $rids, array(), $reset);
}

/**
 * Delete multiple group roles.
 *
 * @param array $rids
 *   The ids of the group roles you want to delete.
 */
function group_role_delete_multiple(array $rids) {
  entity_delete_multiple('group_role', $rids);
}
