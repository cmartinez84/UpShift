<?php
/**
 * @file
 * Plugin to provide access control based upon group permissions.
 *
 * This plugin allows you to allow or deny access based on the permissions
 * a member has in a group.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Group: User has permission'),
  'description' => t('Checks whether a user has a certain permission in a group'),
  'callback' => 'group_group_permission_access_check',
  'default' => array('permissions' => array()),
  'settings form' => 'group_group_permission_access_settings',
  'settings form submit' => 'group_group_permission_access_settings_submit',
  'summary' => 'group_group_permission_access_summary',
  'required context' => array(
    new ctools_context_required(t('Group'), 'group'),
    new ctools_context_required(t('User'), 'user'),
  ),
);

/**
 * Settings form for the group role access plugin.
 */
function group_group_permission_access_settings(&$form, &$form_state, $conf) {
  $group_permissions = module_invoke_all('group_permission');
  foreach ($group_permissions as &$permission) {
    $permission = $permission['title'];
  }

  $form['settings']['permissions'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Group permission'),
    '#description' => t('Only the checked group permissions will be granted access.'),
    '#options' => $group_permissions,
    '#default_value' => $conf['permissions'],
    '#required' => TRUE,
  );

  return $form;
}

/**
 * Compress the roles allowed to the minimum.
 */
function group_group_permission_access_settings_submit($form, &$form_state) {
  $compressed = array_keys(array_filter($form_state['values']['settings']['permissions']));
  $form_state['values']['settings']['permissions'] = $compressed;
}

/**
 * Check for access.
 */
function group_group_permission_access_check($conf, $context) {
  if (empty($context) || count($context) != 2 || empty($context[0]->data) || empty($context[1]->data)) {
    return FALSE;
  }

  $group = $context[0]->data;
  $account = $context[1]->data;

  return user_access('bypass group access') || (bool) array_intersect($conf['permissions'], $group->userPermissions($account->uid));
}

/**
 * Provide a summary description based upon the checked roles.
 */
function group_group_permission_access_summary($conf, $context) {
  $names = array();
  $group_permissions = module_invoke_all('group_permission');

  foreach (array_filter($conf['permissions']) as $permission) {
    $names[] = $group_permissions[$permission]['title'];
  }

  $message_single = '@identifier has group permission "@permissions"';
  $message_plural = '@identifier has one of "@permissions"';
  $replace = array(
    '@permissions' => implode(', ', $names),
    '@identifier' => $context[1]->identifier,
  );

  return format_plural(count($names), $message_single, $message_plural, $replace);
}
