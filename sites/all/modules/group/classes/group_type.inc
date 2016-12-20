<?php
/**
 * @file
 * Defines the Entity API class for group types.
 */

/**
 * Main class for group type entities.
 */
class GroupType extends Entity {

  /**
   * Cache of GroupRole entities.
   *
   * @var array
   */
  protected $entityCache = array();

  /**
   * Resets the internal entity cache.
   *
   * @param array $ids
   *   (optional) If specified, the cache is reset for the GroupRole entities
   *   with the given ids only.
   */
  public function resetCache(array $ids = NULL) {
    if (isset($ids)) {
      foreach ($ids as $id) {
        unset($this->entityCache[$id]);
      }
    }
    else {
      $this->entityCache = array();
    }
  }

  /**
   * Retrieve all possible roles for a group type.
   *
   * @param bool $special
   *   Include special roles 'anonymous', 'outsider' and 'member'.
   * @param bool $reset
   *   Whether to reset the internal cache.
   *
   * @return array
   *   A list of GroupRole entities.
   *
   * @see group_roles()
   */
  public function getRoles($special = TRUE, $reset = FALSE) {
    if (empty($this->entityCache) || $reset) {
      // We're about to fake two type specific roles: outsider and member.
      $values = array('type' => $this->name, 'global' => 0);

      // Fake the anonymous role.
      $values['name'] = 'anonymous';
      $values['label'] = t('Anonymous');
      $values['permissions'] = $this->anonymous_permissions;
      $group_roles['anonymous'] = entity_create('group_role', $values);

      // Fake the outsider role.
      $values['name'] = 'outsider';
      $values['label'] = t('Outsider');
      $values['permissions'] = $this->outsider_permissions;
      $group_roles['outsider'] = entity_create('group_role', $values);

      // Fake the member role.
      $values['name'] = 'member';
      $values['label'] = t('Member');
      $values['permissions'] = $this->member_permissions;
      $group_roles['member'] = entity_create('group_role', $values);

      // Unset the is_new flags to avoid confusion.
      unset($group_roles['anonymous']->is_new, $group_roles['outsider']->is_new, $group_roles['member']->is_new);

      // Add the local roles for this group type.
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'group_role');
      $query->propertyCondition('type', $this->name);
      $query->propertyOrderBy('weight');
      $result = $query->execute();

      if (isset($result['group_role'])) {
        $group_roles += group_roles(array_keys($result['group_role']));
      }

      // Add global roles.
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'group_role');
      $query->propertyCondition('global', 1);
      $query->propertyOrderBy('weight');
      $result = $query->execute();

      if (isset($result['group_role'])) {
        $group_roles += group_roles(array_keys($result['group_role']));
      }

      $this->entityCache = $group_roles;
    }

    $group_roles = $this->entityCache;

    if (!$special) {
      unset($group_roles['anonymous'], $group_roles['outsider'], $group_roles['member']);
    }

    return $group_roles;
  }

  /**
   * Delete all possible roles for a group type.
   *
   * Does not delete global or special (outsider or member) roles.
   *
   * @see group_roles()
   */
  public function removeRoles() {
    $roles = array();

    foreach ($this->getRoles(FALSE) as $name => $group_role) {
      if (empty($group_role->global)) {
        $roles[] = $name;
      }
    }

    // Delete all roles specific to this group type.
    group_role_delete_multiple($roles);

    // Remove all deleted roles from the internal cache.
    $this->resetCache($roles);
  }
}
