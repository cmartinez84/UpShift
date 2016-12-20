<?php
/**
 * @file
 * Entity API related helper functions for group member profile types.
 */

/**
 * Load group member profile types.
 *
 * @param array $names
 *   (optional) An array of machine names to filter on.
 *
 * @return array
 *   An array of GProfileType objects, keyed by their machine names.
 *
 * @see entity_load_multiple_by_name()
 */
function gprofile_types($names = FALSE) {
  return entity_load_multiple_by_name('gprofile_type', $names);
}

/**
 * Retrieve all group member profile type labels.
 *
 * Ideally used for populating option lists.
 *
 * @return array
 *   An array of GProfileType labels, keyed by their machine names.
 */
function gprofile_type_labels() {
  $profile_types = gprofile_types();
  foreach ($profile_types as &$profile_type) {
    $profile_types = $profile_type->label();
  }
  return $profile_types;
}

/**
 * Load a group member profiletype by name.
 *
 * @param string $name
 *   The name of the profile type you want to load.
 *
 * @return GProfileType
 *   The loaded GProfileType entity.
 */
function gprofile_type_load($name) {
  $types = gprofile_types(array($name));
  return reset($types);
}

/**
 * Load multiple group member profile types by id.
 *
 * @param array $tids
 *   (optional) An array of profile type ids to load.
 * @param bool $reset
 *   (optional) Whether to reset the internal cache.
 *
 * @return array
 *   An array of GProfileType entities, keyed by their machine names.
 *
 * @see entity_load()
 */
function gprofile_type_load_multiple(array $tids = array(), $reset = FALSE) {
  return entity_load('gprofile_type', $tids, array(), $reset);
}

/**
 * Delete multiple group member profile types.
 *
 * @param array $tids
 *   The ids of the profile types you want to delete.
 */
function gprofile_type_delete_multiple(array $tids) {
  entity_delete_multiple('gprofile_type', $tids);
}
