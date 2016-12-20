<?php
/**
 * @file
 * Group membership actions form generator.
 *
 * Generates the membership action buttons as defined in by other modules in
 * hook_group_membership_action_info().
 */

/**
 * Generate the membership actions form for a group.
 */
function group_membership_actions_form($form, &$form_state, Group $group, $account) {
  // Make sure this file is available throughout the entire form lifetime.
  form_load_include($form_state, 'inc', 'group', 'forms/group.membership_actions');

  // Retrieve the membership (if it exists).
  $group_membership = $group->getMember($account->uid);

  foreach (group_membership_action_info() as $action => $info) {
    if (isset($info['access callback']) && !call_user_func($info['access callback'], $group, $account, $group_membership)) {
      continue;
    }

    $form[$action] = array(
      '#type' => 'submit',
      '#name' => "op_$action",
      '#value' => $info['label'],
    );
  }

  return $form;
}

/**
 * Submit handler for group_membership_actions_form().
 */
function group_membership_actions_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $group = $form_state['build_info']['args'][0];
  $account = $form_state['build_info']['args'][1];

  // Retrieve the membership (if it exists).
  $group_membership = $group->getMember($account->uid);

  // Call the clicked action's callback.
  foreach (group_membership_action_info() as $action => $info) {
    if (isset($values["op_$action"])) {
      call_user_func($info['action callback'], $group, $account, $group_membership);
    }
  }
}
