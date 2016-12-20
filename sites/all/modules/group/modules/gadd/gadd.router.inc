<?php
/**
 * @file
 * Contains all router functions for the Group add member module.
 */

/**
 * Implements hook_menu().
 */
function gadd_menu() {
  $items['group/%group/add'] = array(
    'title' => 'Add member',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('gadd_add_member_form', 1),
    'access callback' => 'group_access',
    'access arguments' => array('add member', 1),
    'file' => 'forms/gadd.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  return $items;
}
