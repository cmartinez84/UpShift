<?php
/**
 * @file
 * Contains all router functions for the Group Node module.
 */

/**
 * Implements hook_menu().
 */
function gnode_menu() {
  $items['group/%group/node/add/%'] = array(
    'title' => 'Add node',
    'title callback' => 'gnode_add_node_title',
    'title arguments' => array(4),
    'page callback' => 'gnode_node_add',
    'page arguments' => array(1, 4),
    'access callback' => 'gnode_add_node_access',
    'access arguments' => array(1, 4),
    'type' => MENU_CALLBACK,
  );

  return $items;
}

/**
 * Implements hook_menu_local_tasks_alter().
 *
 * Adds node creation local tasks to group/%group.
 */
function gnode_menu_local_tasks_alter(&$data, $router_item, $root_path) {
  // Add action links for node creation on group/%group.
  if ($root_path == 'group/%') {
    $gid = $router_item['original_map'][1];

    foreach (node_type_get_types() as $node_type => $info) {
      $item = menu_get_item("group/$gid/node/add/$node_type");

      // Add one general and one unique class for styling purposes.
      $item['localized_options']['attributes']['class'][] = 'group-local-action';
      $item['localized_options']['attributes']['class'][] = 'group-local-action-' . str_replace('_', '-', $node_type);

      // Add another class to target just Group Node local actions.
      $item['localized_options']['attributes']['class'][] = 'gnode-local-action';

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
 * Title callback for group/%group/node/add/%.
 */
function gnode_add_node_title($node_type) {
  $types = node_type_get_types();
  return t('Add @node', array('@node' => $types[$node_type]->name));
}

/**
 * Access callback for group/%group/node/add/%.
 */
function gnode_add_node_access($group, $node_type) {
  return group_access("create $node_type node", $group);
}
