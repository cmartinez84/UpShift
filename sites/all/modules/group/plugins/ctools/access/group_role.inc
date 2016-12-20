<?php
/**
 * @file
 * Plugin to provide access control based upon group role.
 *
 * This plugin allows you to allow or deny access based on the group roles a
 * member has in a group.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Group: Member has role'),
  'description' => t('Checks whether a user has a certain role in a group'),
  'callback' => 'group_group_role_access_check',
  'default' => array('roles' => array()),
  'settings form' => 'group_group_role_access_settings',
  'settings form submit' => 'group_group_role_access_settings_submit',
  'summary' => 'group_group_role_access_summary',
  'required context' => array(
    new ctools_context_required(t('Group'), 'group'),
    new ctools_context_required(t('User'), 'user'),
  ),
);

/**
 * Settings form for the group role access plugin.
 */
function group_group_role_access_settings(&$form, &$form_state, $conf) {
  if ($options = group_role_labels(TRUE)) {
    asort($options);

    $form['settings']['roles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Group role'),
      '#description' => t('Only the checked group roles will be granted access.'),
      '#options' => $options,
      '#default_value' => $conf['roles'],
      '#required' => TRUE,
    );
  }
  else {
    $form['settings']['info'] = array(
      '#markup' => t('There are no group roles yet, so this will always deny access.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
  }

  return $form;
}

/**
 * Compress the roles allowed to the minimum.
 */
function group_group_role_access_settings_submit($form, &$form_state) {
  $compressed = array_keys(array_filter($form_state['values']['settings']['roles']));
  $form_state['values']['settings']['roles'] = $compressed;
}

/**
 * Check for access.
 */
function group_group_role_access_check($conf, $context) {
  if (empty($context) || count($context) != 2 || empty($context[0]->data) || empty($context[1]->data)) {
    return FALSE;
  }

  $group = $context[0]->data;
  $account = $context[1]->data;

  if ($group_membership = $group->getMember($account->uid)) {
    return (bool) array_intersect($conf['roles'], $group_membership->roles);
  }

  return FALSE;
}

/**
 * Provide a summary description based upon the checked roles.
 */
function group_group_role_access_summary($conf, $context) {
  $names = array();
  $group_roles = group_role_labels(TRUE);

  foreach (array_filter($conf['roles']) as $role) {
    $names[] = $group_roles[$role];
  }

  $message_single = '@identifier has group role "@roles"';
  $message_plural = '@identifier has one of "@roles"';
  $replace = array(
    '@roles' => implode(', ', $names),
    '@identifier' => $context[1]->identifier,
  );

  return format_plural(count($names), $message_single, $message_plural, $replace);
}
