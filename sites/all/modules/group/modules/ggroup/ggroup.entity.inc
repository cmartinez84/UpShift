<?php
/**
 * @file
 * Contains all Entity API functions for the Subgroup module.
 */

/**
 * Implements hook_entity_info_alter().
 */
function ggroup_entity_info_alter(&$entity_info) {
  // Groups can only have one group parent.
  $entity_info['group']['group entity'] = 'single';
}

/**
 * Implements hook_entity_property_info_alter().
 *
 * Informs Drupal of the changes we made to {group_membership}.
 */
function ggroup_entity_property_info_alter(&$info) {
  $group_membership = &$info['group_membership']['properties'];
  $group_membership['parent_membership'] = array(
    'label' => t('Parent membership'),
    'description' => t('The membership from which this one was inherited.'),
    'type' => 'group_membership',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'parent_mid',
  );

  $group_membership['heritage'] = array(
    'label' => t('Membership heritage'),
    'description' => t('The inheritance metadata for the group membership.'),
    'type' => 'struct',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'heritage',
  );

  $group_role_list = array(
    'type' => 'list<group_role>',
    'options list' => 'group_role_labels',
    'setter callback' => 'entity_property_verbatim_set',
    'computed' => TRUE,
  );

  $group_membership['heritage']['property info']['initial'] = array(
    'label' => t('Inherited roles'),
    'description' => t('The group roles that were initially inherited.'),
  ) + $group_role_list;

  $group_membership['heritage']['property info']['granted'] = array(
    'label' => t('Granted roles'),
    'description' => t('The group roles that were granted on top of the inherited ones.'),
  ) + $group_role_list;

  $group_membership['heritage']['property info']['revoked'] = array(
    'label' => t('Inherited roles'),
      'description' => t('The group roles that were revoked from the inherited ones.'),
  ) + $group_role_list;
}

/**
 * Implements hook_entity_insert().
 */
function ggroup_entity_insert($entity, $type) {
  // Check and run inheritance when a subgroup is created.
  if ($type == 'group' && !empty($entity->group)) {
    // Load the parent group.
    $parent = group_load($entity->group);

    // Run inheritance for all of the parent group's memberships.
    foreach ($parent->getMembers() as $group_membership) {
      ggroup_run_member_inheritance($group_membership, array($entity->gid));
    }
  }

  // Check and run inheritance when a membership is created.
  elseif ($type == 'group_membership') {
    // Run inheritance for the membership.
    ggroup_run_member_inheritance($entity);
  }
}

/**
 * Implements hook_entity_update().
 */
function ggroup_entity_update($entity, $type) {
  if ($type == 'group_membership' && $entity->roles_changed) {
    // If an inherited membership is updated, check if it broke inheritance.
    // Furthermore, update the 'heritage' property if necessary.
    if ($entity->status == 'inherited' || $entity->status == 'inherited-overridden') {
      // Retrieve the roles the member should have according to its heritage.
      $heritage = $entity->heritage['initial'];

      // Keep a reference before modifying anything.
      $ref_granted = $entity->heritage['granted'];
      $ref_revoked = $entity->heritage['revoked'];

      // Store whatever roles were changed.
      $granted = $entity->heritage['granted'] = array_diff($entity->roles, $heritage);
      $revoked = $entity->heritage['revoked'] = array_diff($heritage, $entity->roles);

      // Flag an inherited membership as overridden if anything has changed.
      if ($entity->status == 'inherited' && ($granted || $revoked)) {
        $entity->status = 'inherited-overridden';
      }
      // Unflag an overridden membership if it respects its heritage again.
      elseif ($entity->status == 'inherited-overridden' && !$granted && !$revoked) {
        $entity->status = 'inherited';
      }

      // Find out if anything was modified.
      $mod_granted = array_diff($ref_granted, $granted) || array_diff($granted, $ref_granted);
      $mod_revoked = array_diff($ref_revoked, $revoked) || array_diff($revoked, $ref_revoked);

      // Save the membership again and rerun inheritance if there was.
      if ($mod_granted || $mod_revoked) {
        $entity->save();
      }
    }

    // If the membership's roles have changed, we need to run inheritance again.
    ggroup_run_member_inheritance($entity);
  }
}

/**
 * Implements hook_entity_delete().
 *
 * @todo Parent is deleted before children. Causes fatal EMW errors.
 * @see https://drupal.org/node/1789494
 */
function ggroup_entity_delete($entity, $type) {
  if ($type == 'group_membership') {
    // Check for any memberships that had this one as their parent.
    $children = group_memberships(array('parent_mid' => $entity->mid));

    foreach ($children as $group_membership) {
      $granted = $group_membership->heritage['granted'];

      // If the inherited membership was 'clean' or had no extra roles granted,
      // we delete it along with the parent.
      if ($group_membership->status == 'inherited' || empty($granted)) {
        $group_membership->delete();
      }
      // Otherwise, we retain only the additions and unlink the inheritance.
      else {
        $group_membership->roles = $granted;
        $group_membership->status = 'active';
        $group_membership->heritage = NULL;
        $group_membership->parent_mid = NULL;
        $group_membership->save();
      }
    }
  }
}
