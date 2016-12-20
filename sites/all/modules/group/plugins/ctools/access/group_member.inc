<?php
/**
 * @file
 * Plugin to provide access control based upon group membership.
 *
 * This plugin allows you to allow or deny access based on whether a user is
 * a member of a group or not.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Group: User is a member'),
  'description' => t('Checks whether a user is a member of a group'),
  'callback' => 'group_group_member_access_check',
  'summary' => 'group_group_member_access_summary',
  'required context' => array(
    new ctools_context_required(t('Group'), 'group'),
    new ctools_context_required(t('User'), 'user'),
  ),
);

/**
 * Check for access.
 */
function group_group_member_access_check($conf, $context) {
  if (empty($context) || count($context) != 2 || empty($context[0]->data) || empty($context[1]->data)) {
    return FALSE;
  }

  $group = $context[0]->data;
  $account = $context[1]->data;

  return (bool) $group->getMember($account->uid);
}

/**
 * Provide a summary description.
 */
function group_group_member_access_summary($conf, $context) {
  $message = '@identifier is a member of @identifier2';
  $replace = array(
    '@identifier' => $context[1]->identifier,
    '@identifier2' => $context[0]->identifier,
  );

  return t($message, $replace);
}
