<?php
/**
 * @file
 * Contains all Entity API functions for the Group module.
 */

/**
 * Implements hook_entity_info().
 *
 * Defines the entities this module needs to function.
 */
function group_entity_info() {
  $info['group'] = array(
    'label' => t('Group'),
    'plural label' => t('Groups'),
    'description' => t('Groups people and content with roles and permissions.'),
    'entity class' => 'Group',
    'controller class' => 'GroupController',
    'base table' => 'groups',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'gid',
      'bundle' => 'type',
      'label' => 'title',
    ),
    'access callback' => 'group_entity_access',
    'label callback' => 'entity_class_label',
    'uri callback' => 'entity_class_uri',
    // Bundles are set in group_entity_info_alter().
    'bundles' => array(),
    'bundle keys' => array(
      'bundle' => 'name',
    ),
    'view modes' => array(
      'full' => array(
        'label' => t('Group page'),
        'custom settings' => FALSE,
      ),
    ),
    'module' => 'group',
    'admin ui' => array(
      'path' => 'admin/group',
      'file' => 'forms/group.inc',
      'controller class' => 'GroupUIController',
    ),
    'extra fields controller class' => 'GroupExtraFieldsController',
    'views controller class' => 'GroupViewsController',
    // Add Title module support.
    'field replacement' => array(
      'title' => array(
        'field' => array(
          'type' => 'text',
          'cardinality' => 1,
          'translatable' => TRUE,
        ),
        'instance' => array(
          'label' => t('Title'),
          'description' => t('The name of the group.'),
          'required' => TRUE,
          'settings' => array(
            'text_processing' => 0,
          ),
          'widget' => array(
            'weight' => -5,
          ),
          'display' => array(
            'default' => array(
              'type' => 'hidden',
            ),
          ),
        ),
      ),
    ),
  );

  $info['group_type'] = array(
    'label' => t('Group type'),
    'plural label' => t('Group types'),
    'description' => t('Define different group types.'),
    'entity class' => 'GroupType',
    'controller class' => 'GroupTypeController',
    'base table' => 'group_type',
    'fieldable' => FALSE,
    'bundle of' => 'group',
    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'tid',
      'name' => 'name',
      'label' => 'label',
    ),
    'access callback' => 'group_configure_access',
    'label callback' => 'entity_class_label',
    'module' => 'group',
    'admin ui' => array(
      'path' => 'admin/group/type',
      'file' => 'admin/group_type.inc',
      'controller class' => 'GroupTypeUIController',
    ),
    'features controller class' => 'GroupTypeFeaturesController',
    'views controller class' => 'EntityDefaultViewsController',
    // Add Internationalization module support.
    'i18n controller class' => 'EntityDefaultI18nStringController',
  );

  $info['group_role'] = array(
    'label' => t('Group role'),
    'plural label' => t('Group roles'),
    'description' => t('Define different group roles.'),
    'entity class' => 'GroupRole',
    'controller class' => 'GroupRoleController',
    'base table' => 'group_role',
    'fieldable' => FALSE,
    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'rid',
      'name' => 'name',
      'label' => 'label',
    ),
    // Roles are bound to group types.
    'access callback' => 'group_configure_access',
    'label callback' => 'entity_class_label',
    'module' => 'group',
    'admin ui' => array(
      'path' => 'admin/group/role',
      'file' => 'admin/group_role.inc',
      'controller class' => 'GroupRoleUIController',
    ),
    'features controller class' => 'GroupRoleFeaturesController',
    'views controller class' => 'EntityDefaultViewsController',
    // Add Internationalization module support.
    'i18n controller class' => 'EntityDefaultI18nStringController',
  );

  $info['group_membership'] = array(
    'label' => t('Group membership'),
    'plural label' => t('Group memberships'),
    'description' => t('Provides functionality to link Group to User entities.'),
    'entity class' => 'GroupMembership',
    'controller class' => 'GroupMembershipController',
    'base table' => 'group_membership',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'mid',
    ),
    'access callback' => 'group_configure_access',
    'label callback' => 'entity_class_label',
    'uri callback' => 'entity_class_uri',
    'bundles' => array(
      'group_membership' => array(
        'label' => t('Group member'),
        'admin' => array(
          'path' => 'admin/group/member/manage/group_membership',
          'access arguments' => array('configure group module'),
        ),
      ),
    ),
    'view modes' => array(
      'full' => array(
        'label' => t('Member page'),
        'custom settings' => FALSE,
      ),
    ),
    'module' => 'group',
    'admin ui' => array(
      'path' => 'admin/group/member',
      'controller class' => 'GroupMembershipUIController',
    ),
    'extra fields controller class' => 'GroupMembershipExtraFieldsController',
    'views controller class' => 'GroupMembershipViewsController',
  );

  return $info;
}

/**
 * Implements hook_entity_info_alter().
 */
function group_entity_info_alter(&$entity_info) {
  // Informs Drupal that group types are bundles for groups.
  foreach (group_types() as $name => $group_type) {
    $entity_info['group']['bundles'][$name] = array(
      'label' => $group_type->label,
      'admin' => array(
        'path' => 'admin/group/type/manage/%group_type',
        'real path' => 'admin/group/type/manage/' . $name,
        'bundle argument' => 4,
        'access arguments' => array('configure group module'),
      ),
    );
  }

  // Entities default to not being group entities.
  foreach ($entity_info as $entity => $data) {
    $entity_info[$entity] += array(
      'group entity' => FALSE,
    );
  }
}

/**
 * Entity API access callback for groups.
 *
 * @param string $op
 * @param Group $group
 * @param object $account
 * @param string $entity_type
 * @param string $group_type
 *   (optional) The group type to create. Only needs to be specified
 *   when $op is 'create'.
 *
 * @return bool
 */
function group_entity_access($op, Group $group = NULL, $account = NULL, $entity_type = NULL, $group_type = NULL) {
  switch ($op) {
    case 'create':
      return !empty($group_type) && user_access("create $group_type group", $account);

    case 'view':
      return group_access('administer group', $group, $account)
        || group_access('view group', $group, $account);

    case 'update':
      return group_access('administer group', $group, $account)
        || group_access('edit group', $group, $account);

    case 'delete':
      return group_access('administer group', $group, $account)
        || group_access('delete group', $group, $account);
  }
}

/**
 * Entity API access callback for group module configuration entities.
 */
function group_configure_access($op, $entity, $account = NULL, $entity_type = NULL) {
  return user_access('configure group module', $account);
}

/**
 * Entity API access callback for group memberships.
 *
 * Only used for 'view', 'update' and 'delete'; 'create' is usually handled by
 * membership actions such as "Join group" and 'Request membership". Same goes
 * for 'delete', actually, but we only allow administrative deletes here.
 */
function group_membership_access($op, GroupMembership $group_membership = NULL, $account = NULL) {
  if (!empty($group_membership)) {
    $group = group_load($group_membership->gid);

    global $user;
    if (!isset($account)) {
      $account = $user;
    }

    $is_own = $group_membership->uid == $account->uid;

    switch ($op) {
      case 'view':
        return $is_own || group_access('access member pages', $group, $account);

      case 'update':
        return group_access('administer members', $group, $account)
        || ($is_own && group_access('edit own membership', $group, $account));

      case 'delete':
        return group_access('administer members', $group, $account);

      case 'metadata':
        return group_access('access member metadata', $group, $account);
    }
  }

  return FALSE;
}

/**
 * Implements hook_entity_property_info().
 */
function group_entity_property_info() {
  $info = array();

  // Properties for Group entities.
  $group = &$info['group']['properties'];
  $group['gid'] = array(
    'label' => t('Group ID'),
    'description' => t('The unique group ID.'),
    'type' => 'integer',
    'validate callback' => ' entity_metadata_validate_integer_positive',
    'schema field' => 'gid',
  );

  $group['group_type'] = array(
    'label' => t('Group type'),
    'description' => t('The bundle of the group.'),
    'type' => 'group_type',
    'options list' => 'group_type_labels',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'type',
  );

  $group['title'] = array(
    'label' => t('Title'),
    'description' => t('The title of the group.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'title',
  );

  $group['url'] = array(
    'label' => t('URL'),
    'description' => t('The URL of the group.'),
    'type' => 'uri',
    'getter callback' => 'entity_metadata_entity_get_properties',
    'computed' => TRUE,
    'entity views field' => TRUE,
  );

  // Properties for GroupType entities.
  $group_type = &$info['group_type']['properties'];
  $group_type['tid'] = array(
    'label' => t('Group type ID'),
    'description' => t('The unique group type ID.'),
    'type' => 'integer',
    'validate callback' => ' entity_metadata_validate_integer_positive',
    'schema field' => 'tid',
  );

  $group_type['name'] = array(
    'label' => t('Machine name'),
    'description' => t('The machine readable name for the type.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'name',
  );

  $group_type['label'] = array(
    'label' => t('Label'),
    'description' => t('The human readable name for the type.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'label',
    // Add Internationalization module support.
    'translatable' => TRUE,
    'i18n string' => TRUE,
  );

  $group_type['anonymous_permissions'] = array(
    'label' => t('Anonymous permissions'),
    'description' => t('The anonymous permissions for the group type.'),
    'type' => 'list<text>',
    'options list' => 'group_permissions',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'anonymous_permissions',
  );

  $group_type['outsider_permissions'] = array(
    'label' => t('Outsider permissions'),
    'description' => t('The outsider permissions for the group type.'),
    'type' => 'list<text>',
    'options list' => 'group_permissions',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'outsider_permissions',
  );

  $group_type['member_permissions'] = array(
    'label' => t('Member permissions'),
    'description' => t('The member permissions for the group type.'),
    'type' => 'list<text>',
    'options list' => 'group_permissions',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'member_permissions',
  );

  $group_type['config'] = array(
    'label' => t('Group type configuration'),
    'description' => t('The specific configuration for the group type.'),
    'type' => 'struct',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'config',
  );

  $group_type['config']['property info']['creator_roles'] = array(
    'label' => t('Creator roles'),
    'description' => t('The group roles a user gets when creating a group.'),
    'type' => 'list<group_role>',
    'options list' => 'group_role_labels',
    'setter callback' => 'entity_property_verbatim_set',
  );

  $group_type['weight'] = array(
    'label' => t('Weight'),
    'description' => t('The weight of the type compared to others.'),
    'type' => 'integer',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'weight',
  );

  $group_type['status'] = array(
    'label' => t('Entity status'),
    'description' => t('The entity status of the type.'),
    'type' => 'integer',
    'schema field' => 'status',
  );

  $group_type['module'] = array(
    'label' => t('Providing module'),
    'description' => t('The module providing the entity.'),
    'type' => 'text',
    'schema field' => 'module',
  );

  // Properties for GroupRole entities.
  $group_role = &$info['group_role']['properties'];
  $group_role['rid'] = array(
    'label' => t('Group role ID'),
    'description' => t('The unique group role ID.'),
    'type' => 'integer',
    'validate callback' => ' entity_metadata_validate_integer_positive',
    'schema field' => 'rid',
  );

  $group_role['group_type'] = array(
    'label' => t('Group type'),
    'description' => t('The group type the role is coupled to.'),
    'type' => 'group_type',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'type',
  );

  $group_role['name'] = array(
    'label' => t('Machine name'),
    'description' => t('The machine readable name for the role.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'name',
  );

  $group_role['label'] = array(
    'label' => t('Label'),
    'description' => t('The human readable name for the role.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'label',
    // Add Internationalization module support.
    'translatable' => TRUE,
    'i18n string' => TRUE,
  );

  $group_role['global'] = array(
    'label' => t('Global status'),
    'description' => t('Whether the role is global or not.'),
    'type' => 'boolean',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'global',
  );

  $group_role['permissions'] = array(
    'label' => t('Permissions'),
    'description' => t('The permissions for the role.'),
    'type' => 'list<text>',
    'options list' => 'group_permissions',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'permissions',
  );

  $group_role['weight'] = array(
    'label' => t('Weight'),
    'description' => t('The weight of the role compared to others.'),
    'type' => 'integer',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'weight',
  );

  $group_role['status'] = array(
    'label' => t('Entity status'),
    'description' => t('The entity status of the role.'),
    'type' => 'integer',
    'schema field' => 'status',
  );

  $group_role['module'] = array(
    'label' => t('Providing module'),
    'description' => t('The module providing the entity.'),
    'type' => 'text',
    'schema field' => 'module',
  );

  // Properties for GroupMembership entities.
  $group_membership = &$info['group_membership']['properties'];
  $group_membership['mid'] = array(
    'label' => t('Group membership ID'),
    'description' => t('The unique group membership ID.'),
    'type' => 'integer',
    'validate callback' => ' entity_metadata_validate_integer_positive',
    'schema field' => 'mid',
  );

  $group_membership['group'] = array(
    'label' => t('Group'),
    'description' => t('The group the membership belongs to.'),
    'type' => 'group',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'gid',
  );

  $group_membership['user'] = array(
    'label' => t('User'),
    'description' => t('The user the membership belongs to.'),
    'type' => 'user',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'uid',
  );

  // Generated in GroupMembershipController::load().
  $group_membership['roles'] = array(
    'label' => t('Roles'),
    'description' => t('The group roles for the membership.'),
    'type' => 'list<group_role>',
    'options list' => 'group_role_labels',
    'setter callback' => 'entity_property_verbatim_set',
  );

  $group_membership['status'] = array(
    'label' => t('Membership status'),
    'description' => t('The membership status.'),
    'type' => 'token',
    'options list' => 'group_membership_status_types',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'status',
  );

  $group_membership['blocked_status'] = array(
    'label' => t('Membership status (blocked)'),
    'description' => t('The membership status before the member was blocked.'),
    'type' => 'token',
    'options list' => 'group_membership_status_types',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'blocked_status',
  );

  $group_membership['blocked_on'] = array(
    'label' => t('Blocked on'),
    'description' => t('The date when the member was blocked.'),
    'type' => 'date',
    'getter callback' => 'entity_property_verbatim_date_get',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'blocked_on',
  );

  $group_membership['blocked_by'] = array(
    'label' => t('Blocked by'),
    'description' => t('The user who blocked the member.'),
    'type' => 'user',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'blocked_by',
  );

  $group_membership['joined_on'] = array(
    'label' => t('Joined on'),
    'description' => t('The date when the member joined.'),
    'type' => 'date',
    'getter callback' => 'entity_property_verbatim_date_get',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'joined_on',
  );

  $group_membership['requested_on'] = array(
    'label' => t('Requested on'),
    'description' => t('The date when the member requested to join.'),
    'type' => 'date',
    'getter callback' => 'entity_property_verbatim_date_get',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'requested_on',
  );

  $group_membership['approved_on'] = array(
    'label' => t('Approved on'),
    'description' => t('The date when the member was approved.'),
    'type' => 'date',
    'getter callback' => 'entity_property_verbatim_date_get',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'approved_on',
  );

  $group_membership['approved_by'] = array(
    'label' => t('Approved by'),
    'description' => t('The user who approved the member.'),
    'type' => 'user',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'approved_by',
  );

  return $info;
}

/**
 * Implements hook_entity_property_info_alter().
 */
function group_entity_property_info_alter(&$info) {
  // Add the 'group' property to every group entity.
  foreach (entity_get_info() as $type => $e_info) {
    if (!empty($e_info['group entity'])) {
      $is_single = $e_info['group entity'] == 'single';

      $message = $is_single
        ? 'The group the @entity belongs to.'
        : 'The group(s) the @entity belongs to.';

      $properties = &$info[$type]['properties'];
      $properties['group'] = array(
        'label' => $is_single ? t('Parent group') : t('Parent groups'),
        'description' => t($message, array('@entity' => $type)),
        'type' => $is_single ? 'group' : 'list<group>',
        'setter callback' => 'entity_property_verbatim_set',
      );
    }
  }
}

/**
 * Implements hook_entity_load().
 *
 * Add the 'group' key to entity objects that can belong to a group.
 */
function group_entity_load($entities, $type) {
  $entity_info = entity_get_info($type);

  if (!empty($entity_info['group entity'])) {
    $single = $entity_info['group entity'] == 'single';

    // Load group ids outside the loop for performance reasons.
    $gids = group_get_entities_group_ids($type, array_keys($entities));

    foreach ($entities as $entity_id => $entity) {
      if (!empty($gids[$entity_id])) {
        $entity->group = $single ? reset($gids[$entity_id]) : $gids[$entity_id];
      }
    }
  }
}

/**
 * Implements hook_entity_insert().
 */
function group_entity_insert($entity, $type) {
  $entity_info = entity_get_info($type);

  if (!empty($entity_info['group entity'])) {
    group_entity_save($entity, $type);
  }
}

/**
 * Implements hook_entity_update().
 */
function group_entity_update($entity, $type) {
  $entity_info = entity_get_info($type);

  if (!empty($entity_info['group entity'])) {
    // Retrieve the entity id and bundle.
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($type, $entity);

    // Load the original entity to detect changes.
    $original = entity_load_unchanged($type, $entity_id);

    // Gather the original and updated group ids as arrays.
    $original_gids = (isset($original->group)) ? (array) $original->group : array();
    $updated_gids = (isset($entity->group)) ? (array) $entity->group : array();

    // Remove the entity from groups it no longer belongs to.
    if ($gids = array_diff($original_gids, $updated_gids)) {
      foreach (group_load_multiple($gids) as $group) {
        $group->removeEntity($entity_id, $type);
      };
    }

    // Saving takes care of creating new links.
    group_entity_save($entity, $type);
  }
}

/**
 * Helper for hook_entity_insert() and hook_entity_update().
 */
function group_entity_save($entity, $type) {
  if (!empty($entity->group)) {
    // Retrieve the entity id and bundle.
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($type, $entity);

    // Load the original entity to detect changes.
    $original = entity_load_unchanged($type, $entity_id);

    // Gather the original and updated group ids as arrays.
    $original_gids = (isset($original->group)) ? (array) $original->group : array();
    $updated_gids = (isset($entity->group)) ? (array) $entity->group : array();

    // Add the entity to groups that it didn't belong to earlier.
    if ($gids = array_diff($updated_gids, $original_gids)) {
      foreach (group_load_multiple($gids) as $group) {
        $group->addEntity($entity_id, $type, $bundle);
      }
    }
  }
}

/**
 * Implements hook_entity_delete().
 */
function group_entity_delete($entity, $type) {
  $entity_info = entity_get_info($type);

  // Retrieve the entity id and bundle.
  list($entity_id, $revision_id, $bundle) = entity_extract_ids($type, $entity);

  // Delete group memberships.
  if ($type === 'user') {
    $mids = array_keys(group_membership_load_by_user($entity_id));
    group_membership_delete_multiple($mids);
  }

  // Delete group entity mappings.
  if (!empty($entity_info['group entity']) && !empty($entity->group)) {
    foreach (group_load_multiple((array) $entity->group) as $group) {
      $group->removeEntity($entity_id, $type);
    }
  }
}

/**
 * Implements hook_entity_view().
 *
 * Add the membership actions form as an extra field display to groups.
 */
function group_entity_view($entity, $type, $view_mode, $langcode) {
  global $user;

  if ($type == 'group') {
    module_load_include('inc', 'group', 'forms/group.membership_actions');
    $entity->content['membership_actions'] = drupal_get_form('group_membership_actions_form', $entity, $user);
  }
}
