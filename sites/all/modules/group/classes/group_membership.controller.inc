<?php
/**
 * @file
 * Defines the Entity API CRUD class for group memberships.
 */

/**
 * Controller for group membership entities.
 */
class GroupMembershipController extends EntityAPIController {

  /**
   * Add group roles to group memberships upon loading.
   *
   * This will load the memberships' roles before it is passed onto any hooks
   * that act upon entity loading.
   */
  protected function attachLoad(&$queried_entities, $revision_id = FALSE) {
    if ($queried_entities) {
      // Gather all loaded memberships' role data.
      $query = db_select('group_membership_role', 'gmr')
        ->fields('gmr', array('mid', 'role'))
        ->condition('mid', array_keys($queried_entities), 'IN');

      // Gather the roles per group membership id.
      foreach ($query->execute() as $record) {
        $roles[$record->mid][] = $record->role;
      }

      // Add the group roles onto the memberships.
      foreach ($queried_entities as $mid => $group_membership) {
        $group_membership->roles = isset($roles[$mid]) ? $roles[$mid] : array();
      }
    }

    // Invoke the other attachLoad functionality.
    parent::attachLoad($queried_entities, $revision_id);
  }

  /**
   * Overridden to take care of saving membership roles.
   *
   * If this functionality were placed in GroupMembershipController::save(),
   * we'd find ourselves in a catch-22:
   * - On the one hand we need a membership id to save the roles to
   *   {group_membership_role}, requiring us to first save new
   *   GroupMembership entities before saving their roles.
   * - On the other hand, saving an entity causes the insert or update hooks
   *   to be invoked, leaving room for buggy behavior should one try to load
   *   the recently saved entity again (it gets removed from the cache upon
   *   being saved to the database).
   *
   * To solve this without overriding EntityAPIController::save() and without
   * having to fiddle with either module or hook weight, we place the code here
   * because GroupMembershipController::invoke() is called right after the
   * actual insert or update and we are sure to have a membership id already.
   *
   * @see EntityAPIController::save()
   * @see GroupMembershipController::save()
   */
  public function invoke($hook, $group_membership) {
    if ($hook == 'update' || $hook == 'insert') {
      // Below we take care of the 'roles' property that was set on the
      // GroupMembership; saving new roles to and removing revoked roles from
      // the database. For new GroupMembership entities, we can simply grant
      // all of the roles. For existing ones, we need to run some integrity
      // checks.
      if (!empty($group_membership->is_new)) {
        $grant = $group_membership->roles;
      }
      else {
        // Load the original entity to detect changes.
        $original = entity_load_unchanged('group_membership', $group_membership->mid);

        // Flag new roles for granting.
        $grant = array_diff($group_membership->roles, $original->roles);

        // Flag old roles for revoking.
        $revoke = array_diff($original->roles, $group_membership->roles);

        // Inform update hooks if any roles have changed. This is merely a
        // helper property so update hooks don't have to check again.
        $group_membership->roles_changed = $grant || $revoke;
      }

      // Grant the roles by inserting them into {group_membership_role}.
      if (!empty($grant)) {
        $query = db_insert('group_membership_role')->fields(array('mid', 'role'));

        foreach ($grant as $role) {
          $query->values(array(
            'mid' => $group_membership->mid,
            'role' => $role,
          ));
        }

        $query->execute();
      }

      // Revoke the roles by deleting them from {group_membership_role}.
      if (!empty($revoke)) {
        $query = db_delete('group_membership_role')->condition('mid', $group_membership->mid);
        $query->condition('role', $revoke, 'IN');
        $query->execute();
      }
    }

    // Continue running the actual invoke logic.
    parent::invoke($hook, $group_membership);
  }

  /**
   * Delete a group membership.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    if (!empty($ids)) {
      // Delete all associated group membership roles.
      db_delete('group_membership_role')
        ->condition('mid', $ids, 'IN')
        ->execute();
    }

    parent::delete($ids, $transaction);
  }

  /**
   * Save a group membership.
   *
   * The saving of roles takes place in GroupMembershipController::invoke().
   *
   * @see GroupMembershipController::invoke()
   *
   * @todo Validation of added roles.
   */
  public function save($group_membership, DatabaseTransaction $transaction = NULL) {
    if (!isset($group_membership->uid) || $group_membership->uid == 0) {
      throw new EntityMalformedException(t("Group memberships cannot be created for anonymous users."));
    }

    // Clean up the 'roles' property to avoid duplicates.
    $group_membership->roles = array_unique($group_membership->roles);

    return parent::save($group_membership, $transaction);
  }

  /**
   * Create a group membership.
   *
   * We first set up the values that are specific to the group schema
   * but then also run the EntityAPIController counterpart.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return GroupMembership
   *   A new GroupMembership instance.
   */
  public function create(array $values = array()) {
    // Provide defaults that are needed for the db.
    $values += array(
      'roles' => array(),
      'status' => 'active',
    );

    return parent::create($values);
  }
}
