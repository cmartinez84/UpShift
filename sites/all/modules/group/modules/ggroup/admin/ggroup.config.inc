<?php
/**
 * @file
 * Subgroup configuration admin UI.
 */

/**
 * Add subgroup configuration to group type configuration.
 */
function ggroup_config_form($form, &$form_state, GroupType $group_type) {
  // Load the saved configuration.
  $defaults = isset($group_type->config['subgroup'])
    ? $group_type->config['subgroup']
    : array();

  // Get the group type's roles as inheritance source options.
  $sources = $group_type->getRoles();
  foreach ($sources as &$source) {
    $source = $source->label();
  }

  // Anonymous or outsider cannot inherit.
  unset($sources['anonymous'], $sources['outsider']);

  $form['info'] = array(
    '#markup' => t('Below you will find a set of subgroup configuration options for every known group type.<br />When you allow a group type to be a subgroup of this one, you will also be able to configure membership inheritance.'),
  );

  // Set a value tree so we can easily copy over the submitted data later on.
  $form['subgroup']['#type'] = 'vertical_tabs';
  $form['subgroup']['#tree'] = TRUE;

  // Shows a fieldset for each group type, allowing the user to check whether
  // the edited group type can have subgroups of this type added.
  foreach (group_types() as $type => $sub_group_type) {
    $form['subgroup'][$type] = array(
      '#type' => 'fieldset',
      '#title' => $sub_group_type->label(),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $message = 'Enable %group_type as a subgroup of this group type';
    $replace = array('%group_type' => $sub_group_type->label());
    $default = isset($defaults[$type]);

    $form['subgroup'][$type]['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t($message, $replace),
      '#default_value' => $default,
    );

    // Get the subgroup type's roles as inheritance target options.
    $targets = $sub_group_type->getRoles();
    foreach ($targets as &$target) {
      $target = $target->label();
    }

    // Anonymous or outsider cannot be inherited.
    unset($targets['anonymous'], $targets['outsider']);

    // Build a user friendly inheritance table.
    $form['subgroup'][$type]['inheritance'] = array(
      '#theme' => 'ggroup_inheritance_table',
      // Sources and targets are used in the theme function.
      'sources' => array('#type' => 'value', '#value' => $sources),
      'targets' => array('#type' => 'value', '#value' => $targets),
      // We use a form item wrapper so the #states property will work.
      '#type' => 'item',
      '#states' => array(
        'visible' => array(
          ':input[id="edit-subgroup-' . strtr($type, '_', '-') . '-enabled"]' => array('checked' => TRUE),
        ),
      ),
    );

    // Configuration for inheritance is shown on a per group role basis.
    foreach ($sources as $source => $label) {
      $form['subgroup'][$type]['inheritance'][$source] = array(
        '#type' => 'checkboxes',
        '#options' => $targets,
        '#default_value' => isset($defaults[$type][$source]) ? $defaults[$type][$source] : array(),
        // Leave out the 'inheritance' key from the parents.
        '#parents' => array('subgroup', $type, $source),
      );
    }
  }

  $form['sync'] = array(
    '#type' => 'checkbox',
    '#title' => t('Re-synchronize inherited memberships'),
    '#description' => t('Check this to re-synchronize all inherited memberships upon saving the group type.<br />Useful if you previously granted inherited memberships too few or too many permissions.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for ggroup_config_form().
 */
function ggroup_config_form_submit($form, &$form_state) {
  $group_type = $form_state['build_info']['args'][0];

  // Unset the previous configuration so we can create a clean copy below.
  unset($group_type->config['subgroup']);

  // We only store the enabled subgroup types to have cleaner exports.
  foreach ($form_state['values']['subgroup'] as $type => $data) {
    // We have to check whether $data is an array because the vertical tabs set
    // an extra string value to keep track of which tab was active. Prior to PHP
    // 5.4, the empty() check below would incorrectly return FALSE, causing a
    // WSOD when calling unset() on a string offset further down the line.
    if (!is_array($data)) {
      continue;
    }

    // Only act when the 'Enable as subgroup' checkbox was checked.
    if (empty($data['enabled'])) {
      continue;
    }

    // We don't need the enabled flag in exports either. The checkbox is mainly
    // there to allow us to use AJAX or other UX in the configuration form.
    unset($data['enabled']);

    // Same goes for the inheritance key, which only contains the values we
    // needed to pass along to the inheritance table theme function.
    unset($data['inheritance']);

    // By setting data, we flag the subgroup type as enabled.
    $group_type->config['subgroup'][$type] = $data;
  }

  $group_type->save();

  if (!empty($form_state['values']['sync'])) {
    ggroup_run_group_type_inheritance($group_type);
  }
}
