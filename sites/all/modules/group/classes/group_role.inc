<?php
/**
 * @file
 * Defines the Entity API class for group roles.
 */

/**
 * Main class for group role entities.
 */
class GroupRole extends Entity {

  /**
   * Invalidate the parent group type's GroupRole cache.
   *
   * GroupType entities keep an internal cache of associated GroupRole
   * entities. When saving or deleting a GroupRole, we need to invalidate said
   * cache to ensure we do not work with outdated lists.
   */
  public function invalidateTypeCache() {
    // Only invalidate the parent type's cache for local roles.
    if (empty($this->global)) {
      // If the type cannot be loaded, it should mean that this function was
      // run while we are still building an imported type. We therefore do not
      // need to invalidate the cache.
      if ($group_type = group_type_load($this->type)) {
        $group_type->resetCache();
      }
    }
    // If the role is global, we invalidate all caches.
    else {
      foreach (group_types() as $group_type) {
        $group_type->resetCache();
      }
    }
  }

  /**
   * Flag the parent group type as ENTITY_CUSTOM.
   *
   * If the group role belongs to a group type, the group type it belongs
   * to will be flagged as ENTITY_CUSTOM.
   */
  public function flagTypeCustom() {
    // Only flag the parent type when we're dealing with a local group role.
    if (empty($this->global)) {
      // If the type cannot be loaded, it should mean that this function was
      // run while we are still building an imported type. Furthermore, if the
      // group type can be loaded but has the 'is_deleted' property, it means
      // we were about to delete that group type. In both cases, we do not need
      // to save the group type.
      if (($group_type = group_type_load($this->type)) && empty($group_type->is_deleted)) {
        // Saving without $entity->is_rebuild automatically sets ENTITY_CUSTOM.
        $group_type->save();
      }
    }
  }

  /**
   * Grant permissions to a role.
   *
   * @param array $permissions
   *   A list of permission names to grant.
   */
  public function grantPermissions(array $permissions) {
    $permissions = array_merge($this->permissions, $permissions);
    $this->permissions = array_values(array_unique($permissions));

    $this->save();
  }

  /**
   * Revoke permissions from a role.
   *
   * @param array $permissions
   *   (optional) A list of permission names to revoke. Will revoke all
   *   permissions from the group role if $permissions is left blank.
   */
  public function revokePermissions(array $permissions = array()) {
    $this->permissions = !empty($permissions)
      ? array_values(array_diff($this->permissions, $permissions))
      : array();

    $this->save();
  }

  /**
   * Change permissions for a role.
   *
   * This function may be used to grant and revoke multiple permissions at once.
   * For example, when a form exposes checkboxes to configure permissions for a
   * role, the form submit handler may directly pass the submitted values for
   * the checkboxes form element to this function.
   *
   * @param $permissions
   *   An associative array, where the key holds the permission name and the
   *   value determines whether to grant or revoke that permission. Any value
   *   that evaluates to TRUE will cause the permission to be granted. Any
   *   value that evaluates to FALSE will cause the permission to be revoked.
   *   Existing permissions are not changed, unless specified in $permissions.
   *
   * @code
   *   array(
   *     'administer users' => 0,      // Revoke 'administer users'
   *     'administer group' => FALSE,  // Revoke 'administer group'
   *     'create subgroups' => 1,      // Grant 'create subgroups'
   *     'edit group' => TRUE,         // Grant 'edit group'
   *     'view group' => 'view group', // Grant 'view group'
   *   )
   * @endcode
   *
   * @see GroupRole::grantPermissions()
   * @see GroupRole::revokePermissions()
   */
  public function changePermissions(array $permissions) {
    // Grant new permissions for the role.
    $grant = array_filter($permissions);
    if (!empty($grant)) {
      $this->grantPermissions(array_keys($grant));
    }

    // Revoke permissions for the role.
    $revoke = array_diff_assoc($permissions, $grant);
    if (!empty($revoke)) {
      $this->revokePermissions(array_keys($revoke));
    }
  }
}
