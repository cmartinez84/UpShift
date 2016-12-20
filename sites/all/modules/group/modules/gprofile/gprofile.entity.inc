<?php
/**
 * @file
 * Contains all Entity API functions for the Group member profiles module.
 */

/**
 * Implements hook_entity_info().
 *
 * Defines the entities this module needs to function.
 */
function gprofile_entity_info() {
  $info['gprofile'] = array(
    'label' => t('Group member profile'),
    'plural label' => t('Group member profiles'),
    'description' => t('Fieldable profiles for group members.'),
    'entity class' => 'GProfile',
    'controller class' => 'EntityAPIController',
    'base table' => 'gprofile',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'pid',
      'bundle' => 'type',
    ),
    'label callback' => 'entity_class_label',
    'uri callback' => 'entity_class_uri',
    // Bundles are set in gprofile_entity_info_alter().
    'bundles' => array(),
    'bundle keys' => array(
      'bundle' => 'name',
    ),
    'view modes' => array(
      'full' => array(
        'label' => t('Member profile'),
        'custom settings' => FALSE,
      ),
    ),
    'module' => 'gprofile',
    'extra fields controller class' => 'GProfileExtraFieldsController',
  );

  $info['gprofile_type'] = array(
    'label' => t('Group member profile type'),
    'plural label' => t('Group member profile types'),
    'description' => t('Define different member profile types.'),
    'entity class' => 'GProfileType',
    'controller class' => 'GProfileTypeController',
    'base table' => 'gprofile_type',
    'fieldable' => FALSE,
    'bundle of' => 'gprofile',
    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'tid',
      'name' => 'name',
      'label' => 'label',
    ),
    'access callback' => 'gprofile_type_access',
    'label callback' => 'entity_class_label',
    'module' => 'gprofile',
    'admin ui' => array(
      // We set the same path as GroupMembership, but will make sure there
      // are no collisions in GProfileTypeUIController::hook_menu().
      'path' => 'admin/group/member',
      'file' => 'admin/gprofile_type.inc',
      'controller class' => 'GProfileTypeUIController',
    ),
    // Add Internationalization module support.
    'i18n controller class' => 'EntityDefaultI18nStringController',
  );

  return $info;
}

/**
 * Implements hook_entity_info_alter().
 */
function gprofile_entity_info_alter(&$entity_info) {
  // Informs Drupal that profile types are bundles for member profiles.
  foreach (gprofile_types() as $name => $gprofile_type) {
    $entity_info['gprofile']['bundles'][$name] = array(
      'label' => $gprofile_type->label,
      'admin' => array(
        'path' => 'admin/group/member/manage/%gprofile_type',
        'real path' => 'admin/group/member/manage/' . $name,
        'bundle argument' => 4,
        'access arguments' => array('administer group member profile types'),
      ),
    );
  }
}

/**
 * Entity API access callback for group member profile types.
 */
function gprofile_type_access($op, $gprofile_type, $account = NULL, $entity_type = NULL) {
  return user_access('configure group module', $account);
}

/**
 * Implements hook_entity_property_info().
 */
function gprofile_entity_property_info() {
  $info = array();

  // Properties for GProfile entities.
  $profile = &$info['gprofile']['properties'];
  $profile['pid'] = array(
    'label' => t('Group member profile ID'),
    'description' => t('The unique group member profile ID.'),
    'type' => 'integer',
    'validate callback' => ' entity_metadata_validate_integer_positive',
    'schema field' => 'pid',
  );

  $profile['gprofile_type'] = array(
    'label' => t('Group member profile type'),
    'description' => t('The bundle of the member profile.'),
    'type' => 'gprofile_type',
    'options list' => 'gprofile_type_labels',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'type',
  );

  $profile['group_membership'] = array(
    'label' => t('Group member'),
    'description' => t('Group member the profile belongs to.'),
    'type' => 'group_membership',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'mid',
  );

  // Properties for GProfileType entities.
  $profile_type = &$info['gprofile_type']['properties'];
  $profile_type['tid'] = array(
    'label' => t('Group member profile type ID'),
    'description' => t('The unique group member profile type ID.'),
    'type' => 'integer',
    'validate callback' => ' entity_metadata_validate_integer_positive',
    'schema field' => 'tid',
  );

  $profile_type['name'] = array(
    'label' => t('Machine name'),
    'description' => t('The machine readable name for the type.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'name',
  );

  $profile_type['label'] = array(
    'label' => t('Label'),
    'description' => t('The human readable name for the type.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'label',
    // Add Internationalization module support.
    'translatable' => TRUE,
    'i18n string' => TRUE,
  );

  $profile_type['status'] = array(
    'label' => t('Entity status'),
    'description' => t('The entity status of the type.'),
    'type' => 'integer',
    'schema field' => 'status',
  );

  $profile_type['module'] = array(
    'label' => t('Providing module'),
    'description' => t('The module providing the entity.'),
    'type' => 'text',
    'schema field' => 'module',
  );

  return $info;
}

/**
 * Implements hook_entity_property_info_alter().
 */
function gprofile_entity_property_info_alter(&$info) {
  // Generated in gprofile_group_membership_load().
  $group_membership = &$info['group_membership']['properties'];
  $group_membership['profiles'] = array(
    'label' => t('Group member profiles'),
    'description' => t('The member profiles for the membership.'),
    'type' => 'list<gprofile>',
    'computed' => TRUE,
  );
}

/**
 * Implements hook_group_membership_load().
 *
 * Add the 'profiles' key to GroupMembership objects. This is mainly used for
 * building the membership view modes and for entity metadata wrappers.
 *
 * @see gprofile_entity_property_info_alter().
 */
function gprofile_group_membership_load(array $group_memberships) {
  // Gather all loaded memberships' profile data.
  $query = db_select('gprofile', 'gp')
    ->fields('gp', array('mid', 'pid'))
    ->condition('mid', array_keys($group_memberships), 'IN');

  // Gather the profiles per group membership id.
  foreach ($query->execute() as $record) {
    $profiles[$record->mid][] = $record->pid;
  }

  // Add the group profiles onto the memberships.
  foreach ($group_memberships as $mid => $group_membership) {
    $group_membership->profiles = isset($profiles[$mid]) ? $profiles[$mid] : array();
  }
}

/**
 * Implements hook_group_membership_delete().
 */
function gprofile_group_membership_delete(GroupMembership $group_membership) {
  // Do not use the 'profiles' property here because we want to make sure the
  // list of member profiles wasn't manipulated before delete them all.
  foreach (gprofile_load_by_member($group_membership->mid) as $gprofile) {
    $gprofile->delete();
  }
}

/**
 * Implements hook_group_membership_view().
 *
 * Add the member's profiles to the group membership view.
 */
function gprofile_group_membership_view(GroupMembership $group_membership, $view_mode, $langcode) {
  if (!empty($group_membership->profiles)) {
    $group = group_load($group_membership->gid);
    $account = user_load($group_membership->uid);

    foreach (gprofile_load_multiple($group_membership->profiles) as $gprofile) {
      if (group_access("use $gprofile->type profile", $group, $account)) {
        $group_membership->content["profile_$gprofile->type"] = $gprofile->view('full', NULL, TRUE);
      }
    }
  }
}
