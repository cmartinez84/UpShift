<?php
/**
 * @file
 * Defines the Entity API CRUD class for groups.
 */

/**
 * Controller for group entities.
 */
class GroupController extends EntityAPIController {

  /**
   * Overridden to take care of adding the group creator.
   *
   * If this functionality were placed in GroupController::save(), we'd find
   * ourselves in a catch-22:
   * - On the one hand we need a group id to save the creator's GroupMembership
   *   to the database, requiring us to first save new Group entities before
   *   saving their creator memberships.
   * - On the other hand, saving a new entity causes the insert hook to be
   *   invoked, leaving room for buggy behavior should one try to load the
   *   newly created group's members in aforementioned hook.
   *
   * To solve this without overriding EntityAPIController::save() and without
   * having to fiddle with either module or hook weight, we place the code here
   * because GroupController::invoke() is called right after the actual insert
   * and we are sure to have a group id already.
   *
   * @see EntityAPIController::save()
   */
  public function invoke($hook, $group) {
    if ($hook == 'insert') {
      // Retrieve the default roles for this group type.
      $group_type = group_type_load($group->type);
      $roles = isset($group_type->config['creator_roles'])
        ? $group_type->config['creator_roles']
        : array();

      // Add the current user as the first member.
      global $user;
      $group->addMember($user->uid, array('roles' => $roles));

      // On the off chance that someone tried to load the group when acting upon
      // the GroupMembership save above, the field cache for this group needs to
      // be cleared. Otherwise, the newly created group will keep loading
      // without its field data available until the caches are cleared.
      //
      // For now, this is the approach we're taking seeing as the alternative is
      // to copy over EntityAPIController::invoke() entirely, forcing us to keep
      // track of the changes in that function until the world ends.
      cache_clear_all("field:group:$group->gid", 'cache_field');
    }

    // Continue running the actual invoke logic.
    parent::invoke($hook, $group);
  }

  /**
   * Delete a group.
   *
   * As part of the cleanup process, we delete all child entities. This will
   * in turn trigger group_entity_delete() and remove the deleted entities'
   * records as group entity for this group.
   *
   * We do this to keep the generic entity deletion logic in one place. This
   * could be a big performance hit for groups with a lot of content, so we
   * need to carefully monitor how this fares.
   *
   * @see group_entity_delete()
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $mids = array();

    foreach (group_load_multiple($ids) as $group) {
      foreach ($group->getEntities() as $entity_type => $bundles) {
        // It is safe to assume that 'group entity' is either 'single' or
        // 'multiple' because the entities could not belong to a group if
        // 'group entity' were set to FALSE.
        $entity_info = entity_get_info($entity_type);
        $single = $entity_info['group entity'] == 'single';

        foreach ($bundles as $entities) {
          // Keep track of the entities we want to delete along with the group.
          $delete_ids = array();

          // If the entities can only belong to one group, we are safe to just
          // delete all of them along with the group.
          if ($single) {
            $delete_ids = array_keys($entities);
          }
          // If the entity can belong to multiple groups, we need to check
          // on a per-entity basis whether this is the last group that had
          // the entity as its child.
          else {
            foreach ($entities as $entity_id => $entity) {
              if (count($entity->group) == 1) {
                $delete_ids[] = $entity_id;
              }
            }
          }

          entity_delete_multiple($entity_type, $delete_ids);
        }
      }

      // Gather the membership ids to delete.
      $memberships = group_membership_load_by_group($group->gid);
      $mids = array_merge($mids, array_keys($memberships));

      // Add Path module support.
      if (module_exists('path')) {
        $this->deletePath($group);
      }

      // Add Pathauto module support.
      if (module_exists('pathauto') && module_exists('entity_token')) {
        pathauto_entity_path_delete_all('group', $group, "group/$group->gid");
      }
    }

    // Delete group memberships.
    group_membership_delete_multiple($mids);

    parent::delete($ids, $transaction);
  }

  /**
   * Save a group.
   *
   * @see EntityAPIController::save()
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $return = parent::save($entity, $transaction);

    // Add Path module support.
    if (module_exists('path')) {
      $this->savePath($entity);
    }

    // Add Pathauto module support.
    if (module_exists('pathauto') && module_exists('entity_token')) {
      module_load_include('inc', 'group', 'group.pathauto');
      group_pathauto_update_alias($entity, empty($entity->is_new) ? 'update' : 'insert');
    }

    return $return;
  }

  /**
   * Create a group.
   *
   * We first set up the values that are specific to the group schema
   * but then also run the EntityAPIController counterpart.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return Group
   *   A new Group instance.
   */
  public function create(array $values = array()) {
    // Provide defaults that are needed in group_form().
    $values += array(
      'type' => '',
      'title' => '',
    );

    return parent::create($values);
  }

  /**
   * Updates a group path alias upon saving.
   *
   * @param Group $group
   *   The group being saved.
   */
  public function savePath(Group $group) {
    if (isset($group->path)) {
      $path = $group->path;
      $path['alias'] = trim($path['alias']);

      // Delete old alias if user erased it.
      if (empty($group->is_new)) {
        if (!empty($path['pid']) && empty($path['alias'])) {
          path_delete($path['pid']);
        }
      }

      // Only save a non-empty alias.
      if (!empty($path['alias'])) {
        // Ensure fields for programmatic executions.
        $langcode = entity_language('group', $group);

        $path['source'] = 'group/' . $group->gid;
        $path['language'] = isset($langcode) ? $langcode : LANGUAGE_NONE;

        path_save($path);
      }
    }
  }

  /**
   * Removes group path alias upon deletion.
   *
   * @param Group $group
   *   The group being deleted.
   */
  public function deletePath(Group $group) {
    path_delete(array('source' => 'group/' . $group->gid));
  }
}
