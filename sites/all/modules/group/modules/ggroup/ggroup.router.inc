<?php
/**
 * @file
 * Contains all router functions for the Subgroup module.
 */

/**
 * Implements hook_menu().
 */
function ggroup_menu() {
  $items['group/%group/subgroup/add/%group_type'] = array(
    'title' => 'Add subgroup',
    'title callback' => 'ggroup_add_subgroup_title',
    'title arguments' => array(4),
    'page callback' => 'ggroup_add_subgroup_form',
    'page arguments' => array(1, 4),
    'access callback' => 'ggroup_add_subgroup_access',
    'access arguments' => array(1, 4),
    'file' => 'forms/ggroup.inc',
  );

  $items['admin/group/type/manage/%group_type/config/subgroup'] = array(
    'title' => 'Subgroups',
    'description' => 'Configuration regarding subgroups.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ggroup_config_form', 4),
    'access callback' => 'user_access',
    'access arguments' => array('configure group module'),
    'file' => 'admin/ggroup.config.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  return $items;
}

/**
 * Implements hook_menu_local_tasks_alter().
 *
 * Adds subgroup creation local tasks to group/%group.
 */
function ggroup_menu_local_tasks_alter(&$data, $router_item, $root_path) {
  // Add action links for subgroup creation on group/%group.
  if ($root_path == 'group/%') {
    $gid = $router_item['original_map'][1];

    // Set local actions for creating subgroups.
    foreach (group_types() as $group_type => $info) {
      $item = menu_get_item("group/$gid/subgroup/add/$group_type");

      // Add one general and one unique class for styling purposes.
      $item['localized_options']['attributes']['class'][] = 'group-local-action';
      $item['localized_options']['attributes']['class'][] = 'group-local-action-' . str_replace('_', '-', $group_type);

      // Add another class to target just Subgroup local actions.
      $item['localized_options']['attributes']['class'][] = 'ggroup-local-action';

      if ($item['access']) {
        $data['actions']['output'][] = array(
          '#theme' => 'menu_local_action',
          '#link' => $item,
        );
      }
    }
  }
}

/**
 * Title callback for group/%group/subgroup/add/%group_type.
 */
function ggroup_add_subgroup_title(GroupType $group_type) {
  return t('Add @group', array('@group' => $group_type->label()));
}

/**
 * Access callback for group/%group/subgroup/add/%group_type.
 */
function ggroup_add_subgroup_access($group, $group_type) {
  if (empty($group_type)) {
    return FALSE;
  }

  // Check if the parent group supports subgroups of this type.
  $parent_type = group_type_load($group->type);
  if (!isset($parent_type->config['subgroup'][$group_type->name])) {
    return FALSE;
  }

  return group_access("create $group_type->name group", $group);
}
