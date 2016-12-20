<?php
/**
 * @file
 * Defines the Entity API class for group memberships.
 */

/**
 * Main class for group memberships.
 */
class GroupMembership extends Entity {

  /**
   * Defines the entity label if the 'entity_class_label' callback is used.
   */
  protected function defaultLabel() {
    $message = 'Member %account in %group';
    $replace = array(
      '%account' => format_username(user_load($this->uid)),
      '%group' => group_load($this->gid)->label(),
    );

    return t($message, $replace);
  }

  /**
   * Specifies the default uri, which is picked up by uri() by default.
   */
  protected function defaultURI() {
    return array('path' => "group/$this->gid/member/$this->uid" );
  }

  /**
   * Grant roles to a membership.
   *
   * This is merely a helper function that sets the 'roles' property and then
   * saves the GroupMembership. The actual business logic can be found in
   * GroupMembershipController::save().
   *
   * @param array $roles
   *   A list of role names to grant.
   *
   * @see GroupMembershipController::save()
   */
  public function grantRoles(array $roles) {
    // Update the membership's roles property.
    $this->roles = array_merge($this->roles, $roles);

    // Save the membership so the roles are actually granted.
    $this->save();
  }

  /**
   * Revoke roles from a membership.
   *
   * This is merely a helper function that sets the 'roles' property and then
   * saves the GroupMembership. The actual business logic can be found in
   * GroupMembershipController::save().
   *
   * @param array $roles
   *   (optional) A list of role names to revoke. Will revoke all roles
   *   from the group membership if left blank.
   *
   * @see GroupMembershipController::save()
   */
  public function revokeRoles(array $roles = array()) {
    // Update the membership's roles property.
    $this->roles = empty($roles) ? array() : array_diff($this->roles, $roles);

    // Save the membership so the roles are actually revoked.
    $this->save();
  }

  /**
   * Change roles for a membership.
   *
   * This function may be used to grant and revoke multiple roles at once. For
   * example, when a form exposes checkboxes to configure roles for a
   * membership, the form submit handler may directly pass the submitted values
   * for the checkboxes form element to this function.
   *
   * This is merely a helper function that sets the 'roles' property and then
   * saves the GroupMembership. The actual business logic can be found in
   * GroupMembershipController::save().
   *
   * @param $roles
   *   An associative array, where the key holds the role name and the value
   *   determines whether to grant or revoke that role. Any value that
   *   evaluates to TRUE will cause the role to be granted. Any value that
   *   evaluates to FALSE will cause the role to be revoked. Existing roles are
   *   not changed, unless specified in $roles.
   *
   * @code
   *   array(
   *     'group_admin' => 0,         // Revoke 'group_admin'
   *     'group_admin' => FALSE,     // Revoke 'group_admin'
   *     'sub_admin' => 1,           // Grant 'sub_admin'
   *     'sub_admin' => TRUE,        // Grant 'sub_admin'
   *     'sub_admin' => 'sub_admin', // Grant 'sub_admin'
   *   )
   * @endcode
   *
   * @see GroupMembershipController::save()
   */
  public function changeRoles(array $roles) {
    // Find out what roles we want to grant or revoke.
    $grant = array_filter($roles);
    $revoke = array_diff_assoc($roles, $grant);

    // Grant new roles for the membership.
    $this->roles = array_merge($this->roles, array_keys($grant));

    // Revoke roles from the membership.
    $this->roles = array_diff($this->roles, array_keys($revoke));

    // Save the membership so the roles are actually updated.
    $this->save();
  }

  /**
   * Get all roles for a group membership.
   *
   * @return array
   *   An array of GroupRole entities.
   */
  public function getRoles() {
    return group_roles($this->roles);
  }

  /**
   * Get all permissions for a group membership.
   *
   * @return array
   *   An array of group permission names.
   */
  public function getPermissions() {
    // Get the member permissions for the related group type.
    $wrapper = entity_metadata_wrapper('group_membership', $this);
    $permissions = $wrapper->group->group_type->member_permissions->value();

    // Add the permissions that belong to the attached roles.
    foreach ($this->getRoles() as $group_role) {
      $permissions = array_merge($permissions, $group_role->permissions);
    }

    return array_values(array_unique($permissions));
  }

  /**
   * Checks if a membership has a certain group permission.
   *
   * For most circumstances, you should use group_access() to check if a
   * GroupMembership has a certain permission. On some occasions, however, you
   * may want to check for a permission on a GroupMembership instance that
   * hasn't been saved to the database yet.
   *
   * @param string $permission
   *   The group permission to check for.
   *
   * @return bool
   *   Whether the member has access.
   */
  public function hasPermission($permission) {
    // Short-circuit if the membership subject can bypass group access.
    if (user_access('bypass group access', user_load($this->uid))) {
      return TRUE;
    }

    // Otherwise make sure the membership is active.
    $info = group_membership_status_info();
    if (!empty($info[$this->status]['active'])) {
      return in_array($permission, $this->getPermissions());
    }

    return FALSE;
  }

  /**
   * Get all active memberships for a user.
   *
   * @param int $uid
   *   The uid of the user to retrieve memberships for.
   * @param bool $active
   *   (optional) Whether to return active or inactive memberships. Defaults
   *   to TRUE.
   *
   * @return array
   *   An array of GroupMembership objects.
   */
  public static function getByActiveStatus($uid, $active = TRUE) {
    // Gather all statuses that match $active.
    foreach (group_membership_status_info() as $status => $info) {
      if ($info['active'] === $active) {
        $statuses[] = $status;
      }
    }

    // Only query the database if there are statuses to search for.
    if (!empty($statuses)) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'group_membership');
      $query->propertyCondition('uid', $uid);
      $query->propertyCondition('status', $statuses, 'IN');
      $result = $query->execute();

      if (isset($result['group_membership'])) {
        return group_membership_load_multiple(array_keys($result['group_membership']));
      }
    }

    return array();
  }

  /**
   * Get all memberships by status for a user.
   *
   * @param int $uid
   *   The uid of the user to retrieve memberships for.
   * @param string $status
   *   The machine name of the membership status.
   *
   * @return array
   *   An array of GroupMembership objects.
   */
  public static function getByStatus($uid, $status) {
    return group_memberships(array('uid' => $uid, 'status' => $status));
  }
}
