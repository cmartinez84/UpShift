<?php
/**
 * @file
 * Defines the Entity API class for groups.
 */

/**
 * Main class for groups.
 */
class Group extends Entity {

  /**
   * Cache of entities.
   *
   * @var array
   */
  protected $entityCache = array();

  /**
   * Resets the internal entity cache.
   */
  public function resetCache() {
    $this->entityCache = array();
  }

  /**
   * Specifies the default uri, which is picked up by uri() by default.
   */
  protected function defaultURI() {
    return array('path' => 'group/' . $this->gid);
  }

  /**
   * Retrieve all possible roles for a group.
   *
   * @param bool $special
   *   Include special roles 'outsider' and 'member'. Defaults to TRUE.
   * @param bool $reset
   *   Whether to reset the internal cache.
   *
   * @return array
   *   A list of group_role entities.
   *
   * @see GroupType::getRoles()
   */
  public function getRoles($special = TRUE, $reset = FALSE) {
    return group_type_load($this->type)->getRoles($special, $reset);
  }

  /**
   * Add an entity to a group.
   *
   * You could also manipulate the 'group' property on the entity you wish to
   * add. Upon saving that entity, it will automatically be added to the group.
   *
   * @param int $entity_id
   *   The id of the entity.
   * @param string $entity_type
   *   The type of the entity.
   * @param string $bundle
   *   The bundle of the entity.
   */
  public function addEntity($entity_id, $entity_type, $bundle) {
    $entity_info = entity_get_info($entity_type);

    // Can't add entities that aren't group entities.
    if (empty($entity_info['group entity'])) {
      return;
    }

    $query = db_insert('group_entity');
    $query->fields(
      array(
        'gid' => $this->gid,
        'entity_id' => $entity_id,
        'entity_type' => $entity_type,
        'bundle' => $bundle,
      )
    );

    $query->execute();

    // Remove the entity from its controller's internal cache. This way it will
    // have to be loaded again next time it's requested and will thus receive
    // an updated 'group' property.
    entity_get_controller($entity_type)->resetCache(array($entity_id));

    // Reset the internal cache.
    $this->resetCache();
  }

  /**
   * Remove an entity from a group.
   *
   * You could also manipulate the 'group' property on the entity you wish to
   * remove. Upon saving that entity, it will automatically be removed from the
   * group.
   *
   * @param int $entity_id
   *   The id of the entity.
   * @param string $entity_type
   *   The type of the entity.
   */
  public function removeEntity($entity_id, $entity_type) {
    $query = db_delete('group_entity');
    $query->condition('gid', $this->gid);
    $query->condition('entity_id', $entity_id);
    $query->condition('entity_type', $entity_type);
    $query->execute();

    // Remove the entity from its controller's internal cache. This way it will
    // have to be loaded again next time it's requested and will thus receive
    // an updated 'group' property.
    entity_get_controller($entity_type)->resetCache(array($entity_id));

    // Reset the internal cache.
    $this->resetCache();
  }

  /**
   * Get all child entities.
   *
   * @param bool $flat
   *   (optional) Whether or not to flatten the result array.
   * @param bool $reset
   *   (optional) Whether to reset the internal cache.
   *
   * @return array
   *   An array of child entities, keyed by their type, bundle and entity id.
   *   When $flat is set to TRUE, the return value is one big array of
   *   entities, which are keyed numerically and not by their entity id!
   */
  public function getEntities($flat = FALSE, $reset = FALSE) {
    if (empty($this->entityCache) || $reset) {
      $entities = array(
        'flat' => array(),
        'full' => array(),
      );

      $query = db_select('group_entity', 'ge');
      $query->condition('ge.gid', $this->gid);
      $query->addField('ge', 'entity_type');
      $query->addField('ge', 'entity_id');
      $query->addField('ge', 'bundle');

      // Divide the retrieved entity ids by type and bundle.
      foreach ($query->execute() as $record) {
        $entities['full'][$record->entity_type][$record->bundle][] = $record->entity_id;
      }

      // Actually load the retrieved entities.
      foreach ($entities['full'] as $type => $bundles) {
        foreach ($bundles as $bundle => $ids) {
          $entities['full'][$type][$bundle] = entity_load($type, $ids);

          // We run the entity_load() result through array_values() to prevent
          // people from making the false assumption that we still return the
          // entity ids as array keys.
          $entities['flat'] = array_merge($entities['flat'], array_values($entities['full'][$type][$bundle]));
        }
      }

      $this->entityCache = $entities;
    }

    return $flat
        ? $this->entityCache['flat']
        : $this->entityCache['full'];
  }

  /**
   * Get all child entities of a certain type.
   *
   * @param string $entity_type
   *   The type of child entities.
   * @param string $bundle
   *   (optional) The bundle of the entity type.
   *
   * @return array
   *   An array of child entities of a certain type and optionally bundle,
   *   keyed by their entity id.
   */
  public function getEntitiesOfType($entity_type, $bundle = NULL) {
    // We can reuse the cached result of Group::getEntities().
    $children = $this->getEntities();

    // Fetch the child entities of the requested type or
    // initialize an empty array when none were found.
    $children = isset($children[$entity_type]) ? $children[$entity_type] : array();

    if (isset($bundle)) {
      // Fetch the child entities of the requested bundle or
      // initialize an empty array when none were found.
      $entities = isset($children[$bundle]) ? $children[$bundle] : array();
    }

    // To maintain a consistent return value, we combine the
    // entity's bundle arrays if no bundle was specified.
    else {
      $combined = array();

      foreach ($children as $bundle => $entities) {
        $combined += $entities;
      }

      $entities = $combined;
    }

    return $entities;
  }

  /**
   * Add a subgroup to a group.
   *
   * @param Group $group
   *   The subgroup to attach to the group.
   */
  public function addSubgroup(Group $group) {
    drupal_set_message(t('Deprecation notice!<br />Group::addSubgroup() will soon be removed. Please use Group::addEntity() instead.'), 'warning');
    $this->addEntity($group->gid, 'group', $group->type);
  }

  /**
   * Remove a subgroup from a group.
   *
   * @param Group $group
   *   The subgroup to remove from the group.
   */
  public function removeSubgroup(Group $group) {
    drupal_set_message(t('Deprecation notice!<br />Group::removeSubgroup() will soon be removed. Please use Group::removeEntity() instead.'), 'warning');
    $this->removeEntity($group->gid, 'group');
  }

  /**
   * Retrieve all subgroups for a group.
   *
   * This retrieves all descendant subgroups, not just child groups.
   *
   * @return array
   *   A list of group entities.
   */
  public function getSubgroups() {
    drupal_set_message(t('Deprecation notice!<br />Group::getSubgroups() will soon be removed. Please look towards solutions involving the use of use Group::getEntitiesOfType() instead.'), 'warning');

    // Get a list of direct child groups.
    $subgroups = $this->getEntitiesOfType('group');

    foreach ($subgroups as $group) {
      $subgroups += $group->getSubgroups();
    }

    return $subgroups;
  }

  /**
   * Add a user to a group.
   *
   * If the user is anonymous or already a member, nothing changes.
   *
   * @param int $uid
   *   The uid of the member.
   * @param array $values
   *   (optional) Extra values to add to the GroupMembership entity, like the
   *   'roles' property. You cannot overwrite the group id (gid) or user id
   *   (uid) with this method. Leave blank to make the user 'just a member'.
   */
  public function addMember($uid, array $values = array()) {
    if ($uid != 0 && !$this->getMember($uid)) {
      $values = array('gid' => $this->gid, 'uid' => $uid) + $values;
      $group_membership = entity_create('group_membership', $values);
      $group_membership->save();
    }
  }

  /**
   * Remove a member from a group.
   *
   * @param int $uid
   *   The uid of the member.
   */
  public function removeMember($uid) {
    if ($group_membership = $this->getMember($uid)) {
      $group_membership->delete();
    }
  }

  /**
   * Get a single member of a group.
   *
   * @param int $uid
   *   The uid of the member.
   *
   * @return GroupMembership
   *   The GroupMembership of the user.
   */
  public function getMember($uid) {
    return group_membership_load($this->gid, $uid);
  }

  /**
   * Get all members of a group.
   *
   * @return array
   *   An array of GroupMembership entities.
   */
  public function getMembers() {
    return group_membership_load_by_group($this->gid);
  }

  /**
   * Get all members of a group, including subgroup members.
   *
   * @return array
   *   An array of GroupMembership entities.
   */
  public function getMembersRecursive() {
    drupal_set_message(t('Deprecation notice!<br />Group::getMembersRecursive() will soon be removed. Please look towards solutions involving the use of use Group::getMembers() instead.'), 'warning');
    $members = $this->getMembers();

    foreach ($this->getSubgroups() as $subgroup) {
      $members += $subgroup->getMembers();
    }

    return $members;
  }

  /**
   * Get the amount of members in a group.
   *
   * @return int
   *   The total amount of group members.
   */
  public function getMemberCount() {
    $query = db_select('group_membership', 'gm');
    $query->condition('gm.gid', $this->gid);
    $query->addField('gm', 'uid');
    $query->distinct();

    return $query->countQuery()->execute()->fetchField();
  }

  /**
   * Get the amount of members in a group, including subgroup members.
   *
   * @return int
   *   The total amount of group members.
   */
  public function getMemberCountRecursive() {
    drupal_set_message(t('Deprecation notice!<br />Group::getMemberCountRecursive() will soon be removed. Please look towards solutions involving the use of use Group::getMemberCount() instead.'), 'warning');

    // Retrieve the gids of the group's subgroups and add the group's gid.
    $gids = array_merge(array($this->gid), array_keys($this->getSubgroups()));

    $query = db_select('group_membership', 'gm');
    $query->condition('gm.gid', $gids, 'IN');
    $query->addField('gm', 'uid');
    $query->distinct();

    return $query->countQuery()->execute()->fetchField();
  }

  /**
   * Retrieve a user's roles for a group.
   *
   * @param int $uid
   *   The uid of the user to retrieve the roles for.
   *
   * @return array
   *   An array of GroupRole objects, keyed by their machine name.
   */
  public function userRoles($uid) {
    if ($group_membership = $this->getMember($uid)) {
      return $group_membership->getRoles();
    }

    return array();
  }

  /**
   * Check if a given user has a certain role for a group.
   *
   * @param int $uid
   *   The uid of the user to check for.
   * @param string $role
   *   The machine name of the group role.
   *
   * @return bool
   *   Whether the user has the specified role.
   */
  public function userHasRole($uid, $role) {
    return array_key_exists($role, $this->userRoles($uid));
  }

  /**
   * Retrieve a user's permissions for a group.
   *
   * @param int $uid
   *   The uid of the user to retrieve the permissions for.
   *
   * @return array
   *   An array of group permission names.
   */
  public function userPermissions($uid) {
    $group_membership = $this->getMember($uid);

    if ($group_membership) {
      $info = group_membership_status_info();

      if (!empty($info[$group_membership->status]['active'])) {
        return $group_membership->getPermissions();
      }
    }

    return user_is_logged_in()
      ? group_type_load($this->type)->outsider_permissions
      : group_type_load($this->type)->anonymous_permissions;
  }

  /**
   * Check if a given user has a certain group permission.
   *
   * @param int $uid
   *   The uid of the given user.
   * @param string $permission
   *   The name of the group permission.
   *
   * @return bool
   *   Whether the user has the specified permission.
   */
  public function userHasPermission($uid, $permission) {
    return $uid == 1 || in_array($permission, $this->userPermissions($uid));
  }
}
