<?php

/**
 * @file
 * user_relationships_entity.entity.inc
 */

/**
 * Implements hook_entity_info().
 */
function user_relationships_entity_entity_info() {
  $entities = array();

  $entities['user_relationships'] = array(
    'label' => t('User Relationships'),
    'plural label' => t('User Relationships'),
    // Don't use the default DrupalDefaultEntityController class.
    'controller class' => 'UserRelationshipsController',
    'entity class' => 'UserRelationshipsEntity',
    'base table' => 'user_relationships',
    'fieldable' => FALSE,
    'entity keys' => array(
      'id' => 'rid',
      'bundle' => 'type',
    ),
    'module' => 'user_relationships_entity',
    // Disable entity API auto integration.
    'views controller class' => FALSE,
    'rules controller class' => FALSE,
    'metatags' => FALSE,
    'view modes' => array(),
    'access callback' => 'user_relationships_entity_access',
  );

  return $entities;
}

/**
 * Implements hook_entity_property_info_alter().
 */
function user_relationships_entity_entity_property_info_alter(&$info) {
  $properties = &$info['user_relationships']['properties'];

  $properties['requestee_id']['type'] = 'user';

  $properties['approved']['type'] = 'boolean';
}

/**
 * Entity access callback.
 */
function user_relationships_entity_access($op, $entity, $account = NULL, $entity_type = NULL, $type = NULL) {
  if (!$account) {
    global $user;
    $account = user_load($user->uid);
  }

  // view update delete create.
  if ($op === 'create') {

  }
  elseif ($op === 'view') {

  }

  return TRUE;
}

/**
 * Load a user relationship entity.
 */
function user_relationships_entity__load($id, $account = NULL) {
  if (!$account) {
    global $user;
    $account = $user;
  }

  $controller = entity_get_controller('user_relationships');
  $controller->setAccount($account);

  $entities = $controller->load(array($id));
  return !empty($entities) ? reset($entities) : NULL;
}
