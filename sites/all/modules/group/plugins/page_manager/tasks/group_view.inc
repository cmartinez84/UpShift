<?php
/**
 * @file
 * Take over group/%group with Page Manager.
 */

/**
 * Implements hook_PLUGIN_page_manager_tasks().
 */
function group_group_view_page_manager_tasks() {
  return array(
    'task type' => 'page',
    'title' => t('Group template'),
    'admin title' => t('Group template'),
    'admin description' => t('When enabled, this overrides the default Drupal behavior for displaying groups at <em>group/%group</em>. If you add variants, you may use selection criteria such as group type or language or user access to provide different views of groups. If no variant is selected, the default Group view will be used. This page only affects groups viewed as pages, it will not affect groups viewed in lists or at other locations. Also please note that if you are using pathauto, aliases may make a group to be somewhere else, but as far as Drupal is concerned, they are still at group/%group.'),
    'admin path' => 'group/%group',
    'hook menu alter' => 'group_group_view_menu_alter',
    'handler type' => 'context',
    'get arguments' => 'group_group_view_get_arguments',
    'disabled' => variable_get('group_pm_group_view_disabled', TRUE),
    'enable callback' => 'group_group_view_enable',
    'access callback' => 'group_group_view_access_check',
  );
}

/**
 * Tries to hi-jack the group/%group path.
 *
 * @see group_group_view_page_manager_tasks()
 * @see hook_menu_alter()
 */
function group_group_view_menu_alter(&$items, $task) {
  if (variable_get('group_pm_group_view_disabled', TRUE)) {
    return;
  }

  // Override the group view handler for our purpose.
  $callback = $items['group/%group']['page callback'];
  if ($callback == 'group_page' || variable_get('page_manager_override_anyway', FALSE)) {
    $items['group/%group']['page callback'] = 'group_group_view_page';
    $items['group/%group']['file path'] = $task['path'];
    $items['group/%group']['file'] = $task['file'];
  }
  else {
    // automatically disable this task if it cannot be enabled.
    variable_set('group_pm_group_view_disabled', TRUE);
    if (!empty($GLOBALS['group_pm_enabling_group_view'])) {
      drupal_set_message(t('Page manager module is unable to enable group/%group because some other module already has overridden with %callback.', array('%callback' => $callback)), 'error');
    }
  }
}

/**
 * Entry point for our overridden group view.
 *
 * This function asks its assigned handlers who, if anyone, would like
 * to run with it. If no one does, it passes through to the standard
 * group view, which is group_page().
 *
 * @param Group $group
 *   The group object.
 *
 * @return array
 *   A render array suitable for use by drupal_render().
 *
 * @see group_group_view_menu_alter()
 */
function group_group_view_page($group) {
  // Keep track of whether we need to set the page title and some <head> links.
  $set_head = TRUE;

  // Load the necessary files for context and task handling.
  ctools_include('context');
  ctools_include('context-task-handler');

  // Load the task plugin
  $task = page_manager_get_task('group_view');

  // Retrieve the Group context given the Group argument.
  $contexts = ctools_context_handler_get_task_contexts($task, '', array($group));

  // Try to receive output from the task handlers.
  $output = ctools_context_handler_render($task, '', $contexts, array($group->gid));

  // If no handlers were defined, fall back to either group_page() or another
  // function defined in hook_page_manager_override().
  if ($output === FALSE) {
    $function = 'group_page';

    foreach (module_implements('page_manager_override') as $module) {
      $override_function = module_invoke($module, 'page_manager_override', $function);

      if (function_exists($override_function)) {
        $function = $override_function;
        break;
      }
    }

    if ($function == 'group_page') {
      module_load_include('inc', 'group', 'pages/group');

      // No need to set the head here as group_page() will do the same.
      $set_head = FALSE;
    }

    $output = $function($group);
  }

  // Set the page title and canonical and shortlink head links. Copied verbatim
  // from group_page(). See that function for a more documented version.
  if ($set_head) {
    drupal_set_title($group->label());
    $uri = entity_uri('group', $group);
    drupal_add_html_head_link(array('rel' => 'canonical', 'href' => url($uri['path'], $uri['options'])), TRUE);
    drupal_add_html_head_link(array('rel' => 'shortlink', 'href' => url($uri['path'], array_merge($uri['options'], array('alias' => TRUE)))), TRUE);
  }

  return $output;
}

/**
 * Callback to get arguments provided by this task handler.
 *
 * @see group_group_view_page_manager_tasks()
 */
function group_group_view_get_arguments($task, $subtask_id) {
  return array(
    array(
      'keyword' => 'group',
      'identifier' => t('Group being viewed'),
      'id' => 1,
      'name' => 'entity_id:group',
      'settings' => array(),
    ),
  );
}

/**
 * Callback to enable or disable the page from the UI.
 *
 * @see group_group_view_page_manager_tasks()
 */
function group_group_view_enable($cache, $disabled) {
  variable_set('group_pm_group_view_disabled', $disabled);

  // Track the status so group_group_view_menu_alter() can throw an error when
  // it cannot swap out the group view page with this task plugin.
  if (!$disabled) {
    $GLOBALS['group_pm_enabling_group_view'] = TRUE;
  }
}

/**
 * Callback to determine if the page is accessible.
 *
 * @param array $task
 *   The task plugin.
 * @param string $subtask_id
 *   The subtask id.
 * @param array $contexts
 *   The contexts loaded for the task.
 *
 * @return bool
 *   TRUE if the current user can access the page.
 *
 * @see group_group_view_page_manager_tasks()
 */
function group_group_view_access_check($task, $subtask_id, $contexts) {
  $context = reset($contexts);
  return group_entity_access('view', $context->data);
}
