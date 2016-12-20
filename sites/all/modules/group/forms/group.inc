<?php
/**
 * @file
 * Group editing UI.
 */

/**
 * Generates the group editing form.
 *
 * Pathauto support is also added here, albeit with a twist.
 *
 * @see group_install()
 */
function group_form($form, &$form_state, Group $group, $op = 'edit') {
  $group_type = group_type_load($group->type);

  if ($op == 'add') {
    drupal_set_title(t('Create @name', array('@name' => $group_type->label())));
  }
  elseif ($op == 'edit') {
    drupal_set_title(t('Edit @title', array('@title' => $group->label())));
  }

  // A bundle property is needed for entity_form_field_validate().
  $form['type'] = array(
    '#type' => 'value',
    '#value' => $group->type,
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $group->label(),
    '#description' => t('The name of the group.'),
    '#required' => TRUE,
    '#maxlength' => 255,
    '#size' => 30,
  );

  // Insert vertical tabs to add admin options to.
  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  // Add Path module support.
  if (module_exists('path')) {
    group_attach_path_form($group, $form, $form_state);
  }

  // Load fields based on group type, this will also enable Pathauto support by
  // invoking the hook implementation pathauto_field_attach_form().
  field_attach_form('group', $group, $form, $form_state, entity_language('group', $group));

  // Tweak the Pathauto additions so the user is notified the checkbox won't do
  // anything unless they enable Entity tokens. See group_install() for info.
  if (isset($form['path']['pathauto']) && !module_exists('entity_token')) {
    $group->path['pathauto'] = FALSE;

    // Only disable the whole '!enabled' state if there is one entry in it.
    if (count($form['path']['alias']['#states']['!enabled']) == 1) {
      unset($form['path']['alias']['#states']['!enabled']);
    }
    else {
      unset($form['path']['alias']['#states']['!enabled']['input[name="path[pathauto]"]']);
    }

    // Disable the Pathauto checkbox and add a message.
    $message = t('You need to enable the Entity tokens module for Pathauto to work with Group.');
    $form['path']['pathauto']['#default_value'] = 0;
    $form['path']['pathauto']['#disabled'] = TRUE;
    $form['path']['pathauto']['#description'] .= '<br /><strong>' . t('Please note:') . '</strong> ' . $message;
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save @group_type', array('@group_type' => $group_type->label())),
    '#weight' => 40,
  );

  if ($op == 'edit' && group_access('delete group', $group)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete @group_type', array('@group_type' => $group_type->label())),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('group_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Form API validate callback for the group form.
 */
function group_form_validate($form, &$form_state) {
  entity_form_field_validate('group', $form, $form_state);
}

/**
 * Form API submit callback for the group form.
 */
function group_form_submit($form, &$form_state) {
  $group = entity_ui_form_submit_build_entity($form, $form_state);
  $group->save();

  $message = 'The @group_type %group was saved successfully.';
  $replace = array(
    '@group_type' => group_type_load($group->type)->label(),
    '%group' => $group->label(),
  );

  drupal_set_message(t($message, $replace));
  $form_state['redirect'] = "group/$group->gid";
}

/**
 * Form API submit callback for the delete button.
 */
function group_form_submit_delete($form, &$form_state) {
  $form_state['redirect'] = 'group/' . $form_state['group']->gid . '/delete';
}

/**
 * Add in Path module support to the Group form.
 */
function group_attach_path_form(Group $group, &$form, &$form_state) {
  $path = array();

  if (!empty($group->gid)) {
    $conditions = array('source' => 'group/' . $group->gid);
    $langcode = entity_language('group', $group);

    if ($langcode != LANGUAGE_NONE) {
      $conditions['language'] = $langcode;
    }

    $path = path_load($conditions);
    if ($path === FALSE) {
      $path = array();
    }
  }

  $path += array(
    'pid' => NULL,
    'source' => isset($group->gid) ? 'group/' . $group->gid : NULL,
    'alias' => '',
    'language' => isset($langcode) ? $langcode : LANGUAGE_NONE,
  );

  $form['path'] = array(
    '#type' => 'fieldset',
    '#title' => t('URL path settings'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($path['alias']),
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('path-form'),
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'path') . '/path.js'),
    ),
    '#access' => user_access('create url aliases') || user_access('administer url aliases'),
    '#weight' => 30,
    '#tree' => TRUE,
    '#element_validate' => array('path_form_element_validate'),
  );

  $form['path']['alias'] = array(
    '#type' => 'textfield',
    '#title' => t('URL alias'),
    '#default_value' => $path['alias'],
    '#maxlength' => 255,
    '#description' => t('Optionally specify an alternative URL by which this group can be accessed. Use a relative path and don\'t add a trailing slash or the URL alias won\'t work.'),
  );

  // Add in the fixed values.
  foreach (array('pid', 'source', 'language') as $fixed) {
    $form['path'][$fixed] = array(
      '#type' => 'value',
      '#value' => $path[$fixed],
    );
  }
}