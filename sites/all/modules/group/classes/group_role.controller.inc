<?php
/**
 * @file
 * Defines the Entity API CRUD class for group roles.
 */

/**
 * Controller for group role entities.
 */
class GroupRoleController extends EntityAPIControllerExportable {

  /**
   * Delete a group role.
   *
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    if (!empty($ids)) {
      foreach (group_roles($ids) as $group_role) {
        // Invalidate the parent group type's GroupRole cache.
        $group_role->invalidateTypeCache();

        // Flag parent type as ENTITY_CUSTOM.
        $group_role->flagTypeCustom();

        // Add Internationalization module support.
        if (module_exists('i18n_string')) {
          i18n_string_object_remove('group_role', $group_role);
        }
      }
    }

    parent::delete($ids, $transaction);
  }

  /**
   * Save a group role.
   *
   * @see EntityAPIController::save()
   */
  public function save($group_role, DatabaseTransaction $transaction = NULL) {
    $is_new = !empty($group_role->is_new);
    $is_special = in_array($group_role->name, array('anonymous', 'outsider', 'member'));

    // Invalidate the parent type's GroupRole cache whenever we save a new
    // group role or alter a special group role (anonymous, outsider or
    // member). The latter case is required because special group roles are
    // never actually saved to the db and thus need to be rebuilt after every
    // change to avoid serving outdated special roles from the cache.
    if ($is_new || $is_special) {
      $group_role->invalidateTypeCache();
    }

    // If we save a special group role (anonymous, outsider or member), we
    // don't actually save the role but attach its permissions back onto the
    // group type.
    if ($is_special) {
      $key = $group_role->name . '_permissions';

      $group_type = group_type_load($group_role->type);
      $group_type->$key = $group_role->permissions;
      $group_type->save();

      return;
    }

    // If we save a group role we call GroupRole::flagTypeCustom() which in turn
    // will check whether or not there is a parent group type that needs to be
    // flagged as ENTITY_CUSTOM.
    $group_role->flagTypeCustom();

    // Add Internationalization module support.
    if (module_exists('i18n_string')) {
      i18n_string_object_update('group_role', $group_role);
    }

    return parent::save($group_role, $transaction);
  }

  /**
   * Create a group role.
   *
   * We first set up the values that are specific to the group role schema
   * but then also run the EntityAPIControllerExportable counterpart.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return GroupRole
   *   A new GroupRole instance.
   */
  public function create(array $values = array()) {
    // Provide defaults that are needed in group_role_form().
    $values += array(
      'type' => '',
      'name' => '',
      'label' => '',
      'global' => 1,
      'permissions' => array(),
    );

    return parent::create($values);
  }
}
