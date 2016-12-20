<?php
/**
 * @file
 * Defines the Entity API UI class for groups.
 */

/**
 * UI class for groups.
 */
class GroupUIController extends EntityDefaultUIController {

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    // Make this an entry in the Management menu.
    $items[$this->path] = array(
      'title' => 'Groups',
      'description' => 'Find and manage groups.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_overview_form', 'group'),
      'access arguments' => array('access group overview'),
      'file' => 'includes/entity.ui.inc',
      'weight' => -9,
    );

    // Make this an entry in the Management menu.
    $items["$this->path/overview"] = array(
      'title' => 'Groups',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    $items['group/autocomplete'] = array(
      'title' => 'Group autocomplete',
      'page callback' => 'group_autocomplete',
      // TRUE because of individual 'view' access checks on every group.
      'access callback' => TRUE,
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    $items['group/autocomplete/type/%group_type'] = array(
      'title' => 'Group type specific autocomplete',
      'page callback' => 'group_autocomplete_by_type',
      'page arguments' => array(3),
      // TRUE because of individual 'view' access checks on every group.
      'access callback' => TRUE,
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    $items['group/add'] = array(
      'title' => 'Add group',
      'page callback' => 'group_add_page',
      'access callback' => '_group_add_access',
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
    );

    foreach (group_types() as $name => $group_type) {
      $group = entity_create('group', array('type' => $name));

      $items["group/add/$name"] = array(
        'title' => $group_type->label,
        'description' => "Create $group_type->label",
        'page callback' => 'entity_ui_get_form',
        'page arguments' => array('group', $group, 'add'),
        'access callback' => 'group_entity_access',
        'access arguments' => array('create', NULL, NULL, 'group', $name),
        'file' => 'forms/group.inc',
        'file path' => drupal_get_path('module', 'group'),
      );
    }

    $items['group/%group'] = array(
      'title callback' => 'group_page_title',
      'title arguments' => array(1),
      'page callback' => 'group_page',
      'page arguments' => array(1),
      'access callback' => 'group_entity_access',
      'access arguments' => array('view', 1),
      'file' => 'pages/group.inc',
      'file path' => drupal_get_path('module', 'group'),
    );

    $items['group/%group/view'] = array(
      'title' => 'View',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    $items['group/%group/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'entity_ui_get_form',
      'page arguments' => array('group', 1),
      'access callback' => 'group_entity_access',
      'access arguments' => array('update', 1),
      'file' => 'forms/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => -5,
      // @todo Properly implement contextual links.
      // 'context' => MENU_CONTEXT_INLINE,
    );

    $items['group/%group/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_operation_form', 'group', 1, 'delete'),
      'access callback' => 'group_entity_access',
      'access arguments' => array('delete', 1),
      'file' => 'forms/group.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }

  /**
   * Builds the group overview form.
   *
   * @todo Dynamic pager amount.
   */
  public function overviewForm($form, &$form_state) {
    // Load the file holding most of the form part builders.
    form_load_include($form_state, 'inc', 'group', 'admin/group');

    // If the form was rebuilt from an operation, we show that form instead.
    if ($form_state['rebuild']) {
      $args = array_merge(array($form, &$form_state), $form_state['rebuild_info']['args']);
      return call_user_func_array($form_state['rebuild_info']['callback'], $args);
    }

    // The primary submit handler for this form is the filter.
    $form['#submit'] = array('group_filters_form_submit');

    // Build the overview form with filters and bulk operations.
    $form['filters'] = group_filters_form();
    $form['options'] = group_options_form();
    $form['groups'] = group_groups_form($this->overviewPagerLimit);
    $form['pager'] = array('#markup' => theme('pager'));

    return $form;
  }

  /**
   * Builds the operation form.
   *
   * Overrides the default to only allow the deletion of groups.
   */
  public function operationForm($form, &$form_state, $group, $op) {
    if ($op == 'delete') {
      $form['info'] = array(
        '#markup' => t('By deleting this group you will delete all of its subgroups and content as well.'),
        '#suffix' => '<br />',
      );

      $message = 'Are you sure you want to delete the group %label?';
      $replace = array(
        '%label' => $group->label(),
      );

      return confirm_form($form, t($message, $replace), "group/$group->gid", NULL, t('Delete'));
    }

    drupal_not_found();
    exit;
  }

  /**
   * Operation form submit callback.
   */
  public function operationFormSubmit($form, &$form_state) {
    parent::operationFormSubmit($form, $form_state);

    // Redirect to the front page after deleting a group.
    $form_state['redirect'] = '<front>';
  }

  /**
   * Applies an operation to the given entity.
   *
   * Overrides the default to only allow the deletion of groups.
   */
  public function applyOperation($op, $group) {
    if ($op == 'delete') {
      $title = $group->label();
      $group->delete();

      watchdog('group', 'Deleted %title.', array('%title' => $title));
      return t('Deleted %title.', array('%title' => $title));
    }

    return FALSE;
  }
}
