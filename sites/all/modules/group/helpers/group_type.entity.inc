<?php
/**
 * @file
 * Entity API related helper functions for group types.
 */

/**
 * Load group types.
 *
 * @param array $names
 *   (optional) An array of machine names to filter on.
 *
 * @return array
 *   An array of GroupType objects, keyed by their machine names.
 *
 * @see entity_load_multiple_by_name()
 */
function group_types($names = FALSE) {
  return entity_load_multiple_by_name('group_type', $names);
}

/**
 * Retrieve all group type labels.
 *
 * Ideally used for populating option lists.
 *
 * @return array
 *   An array of GroupType labels, keyed by their machine names.
 */
function group_type_labels() {
  $group_types = group_types();
  foreach ($group_types as &$group_type) {
    $group_type = $group_type->label();
  }
  return $group_types;
}

/**
 * Load a group type by name.
 *
 * @param string $name
 *   The name of the group type you want to load.
 *
 * @return GroupType
 *   The loaded GroupType entity.
 */
function group_type_load($name) {
  $types = group_types(array($name));
  return reset($types);
}

/**
 * Load multiple group types by id.
 *
 * @param array $tids
 *   (optional) An array of group type ids to load.
 * @param bool $reset
 *   (optional) Whether to reset the internal cache.
 *
 * @return array
 *   An array of GroupType entities, keyed by their machine names.
 *
 * @see entity_load()
 */
function group_type_load_multiple(array $tids = array(), $reset = FALSE) {
  return entity_load('group_type', $tids, array(), $reset);
}

/**
 * Delete multiple group types.
 *
 * @param array $tids
 *   The ids of the group types you want to delete.
 */
function group_type_delete_multiple(array $tids) {
  entity_delete_multiple('group_type', $tids);
}
