<?php
/**
 * @file
 * Defines the Entity API CRUD class for group member profile types.
 */

/**
 * Controller for group member profile type entities.
 */
class GProfileTypeController extends EntityAPIControllerExportable {

  /**
   * Delete a group member profile type.
   *
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $pids = array();

    foreach (gprofile_types($ids) as $gprofile_type) {
      // If an entity is deleted while it was flagged as ENTITY_IN_CODE, it
      // means the entity was either reverted or really deleted. By checking
      // for the 'is_rebuild' property, we know it was deleted from within
      // _entity_defaults_rebuild() which only deletes the entity if the
      // default it came from is no longer available. In any other case, we
      // are dealing with a revert or a manual deletion which will only result
      // in the entity being rebuilt upon next cache clear.
      $entity_in_code = entity_has_status('gprofile_type', $gprofile_type, ENTITY_IN_CODE);
      $entity_rebuilt = !empty($gprofile_type->is_rebuild);

      // Set this on the group type so other modules can use it.
      $gprofile_type->is_revert = $entity_in_code && !$entity_rebuilt;

      // Gather the group ids of all groups of the deleted type.
      if (!$gprofile_type->is_revert) {
        $gprofiles = gprofile_load_by_type($gprofile_type->name);
        $pids = array_merge($pids, array_keys($gprofiles));
      }

      // Add Internationalization module support.
      if (module_exists('i18n_string')) {
        i18n_string_object_remove('gprofile_type', $gprofile_type);
      }
    }

    // Delete all profiles of the deleted, but not reverted types.
    if (!empty($pids)) {
      gprofile_delete_multiple($pids);
    }

    // Delete the profile types after setting our flags so those flags are still
    // being passed on to other modules implementing hook_gprofile_type_delete().
    parent::delete($ids, $transaction);
  }


  /**
   * Save a group member profile type.
   *
   * @see EntityAPIController::save()
   */
  public function save($gprofile_type, DatabaseTransaction $transaction = NULL) {
    // Add Internationalization module support.
    if (module_exists('i18n_string')) {
      i18n_string_object_update('gprofile_type', $gprofile_type);
    }

    return parent::save($gprofile_type, $transaction);
  }

  /**
   * Create a profile type.
   *
   * We first set up the values that are specific to the profile type schema
   * but then also run the EntityAPIControllerExportable counterpart.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return GProfileType
   *   A new GProfileType instance.
   */
  public function create(array $values = array()) {
    // Provide defaults that are needed in gprofile_type_form().
    $values += array(
      'name' => '',
      'label' => '',
    );

    return parent::create($values);
  }
}
