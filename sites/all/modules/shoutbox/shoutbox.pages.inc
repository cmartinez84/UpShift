<?php

/**
 * @file
 * Page callbacks for the shoutbox module.
 */

/**
 * Admin settings form.
 */
function shoutbox_admin_settings($form, &$form_state) {
  //add css to admin section
  drupal_add_css(drupal_get_path('module', 'shoutbox') . '/shoutbox.css');

  /***start - display_settings****/
  $form['display_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display settings'),
    '#collapsible' => TRUE,
  );
  $form['display_settings']['shoutbox_showamount_block'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of posts to show in the block'),
    '#default_value' => variable_get('shoutbox_showamount_block', 10),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#description' => t('Set the number of shoutbox posts to show in the block.'),
  );
  $form['display_settings']['shoutbox_showamount_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of posts to show on the page'),
    '#default_value' => variable_get('shoutbox_showamount_page', 30),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#description' => t('Set the number of shoutbox posts to show on the page.'),
  );
  $form['display_settings']['shoutbox_ascending'] = array(
    '#type' => 'checkbox',
    '#title' => t('Post newest shouts on top'),
    '#default_value' => variable_get('shoutbox_ascending', 1),
    '#description' => t('When checked, new shouts will appear on the top instead of the bottom.'),
  );
  $form['display_settings']['shoutbox_defaultname'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default the name field to the logged in user name'),
    '#default_value' => variable_get('shoutbox_defaultname', 1),
    '#description' => t('When checked, "Your name/nick" will be replaced by the logged in user name'),
  );
  $form['display_settings']['shoutbox_widget_type'] = array(
    '#type' => 'radios',
    '#title' => t('Input form type'),
    '#default_value' => variable_get('shoutbox_widget_type', 'textfield'),
    '#description' => t('Choose the type of form widget used for entering a shout.'),
    '#options' => array(
      'textfield' => t('Textfield'),
      'textarea' => t('Textarea'),
    ),
  );

  // set the default message of the shoutfield
  $form['display_settings']['shoutbox_default_message'] = array(
    '#type' => 'textfield',
    '#title' => t('The default message of the shoutfield'),
    '#default_value' => variable_get('shoutbox_default_message', DEFAULTMSGTEXT),
    '#size' => 60,
    '#maxlength' => 255,
    '#required' => FALSE,
    '#description' => t("Set the default message in the inputfield of the shoutbox. If you don't want to show a message, leave this input field empty."),
    '#attributes' => array('class' => array('shoutbox_default_message')),
  );

  $form['display_settings']['shoutbox_time_format'] = array(
    '#type' => 'select',
    '#title' => t('Time format'),
    '#default_value' => variable_get('shoutbox_time_format', 'ago'),
    '#description' => t('Choose the format which shout times will be rendered in.'),
    '#options' => array(
      'ago' => t('Time ago (1 hour 15 minutes ago)'),
      'small' => t('Small !time', array('!time' => '(' . format_date(REQUEST_TIME, 'short') . ')')),
      'medium' => t('Medium !time', array('!time' => '(' . format_date(REQUEST_TIME, 'medium') . ')')),
      'large' => t('Large !time', array('!time' => '(' . format_date(REQUEST_TIME, 'long') . ')')),
    ),
  );
  $form['display_settings']['shoutbox_restrict_general_shouts'] = array(
    '#type' => 'checkbox',
    '#title' => t('Restrict general shouts'),
    '#default_value' => variable_get('shoutbox_restrict_general_shouts', 1),
    '#description' => t('When checked, shoutboxes outside of specific contexts will be restricted to general shouts. There are modules that utilize the Shoutbox API, such as Shoutbox group, that will enter shouts meant to only be displayed in certain locations. Checking this will insure that those shouts are only displayed by the modules responsible for creating them.'),
  );

  $choices = array();
  // Generate choices for profile fields or custom fields for user
  if (module_exists('profile') && db_table_exists('profile_field')) {
    $fields = db_query("SELECT name FROM {profile_field} ORDER BY name ASC");
    foreach ($fields as $field) {
      $choices['profile!' . $field->name] = $field->name;
    }

    $form['display_settings']['shoutbox_profile_name'] = array(
      '#type' => 'select',
      '#title' => t('Use profile field for user name'),
      '#options' => array(0 => '- None -') + $choices,
      '#default_value' => variable_get('shoutbox_profile_name', 0),
      '#description' => t('Select an existing custom profile field to use as the user name.'),
    );
  }
  elseif (db_table_exists('field_config') && db_table_exists('field_config_instance')) {
    $fields = db_query("SELECT fi.field_name,fi.id FROM {field_config} as f,{field_config_instance} as fi
                          WHERE f.id=fi.field_id AND f.type='text'
                          AND fi.entity_type='user' AND fi.bundle='user' ORDER BY field_name ASC");

    foreach ($fields as $field) {
      $field_name = str_replace("field_", "", $field->field_name);
      $choices['custom<split>' . $field_name] = $field_name;
    }
  }
  //if custom fields found add select box
  if (count($choices) > 0) {
    $form['display_settings']['shoutbox_profile_name'] = array(
      '#type' => 'select',
      '#title' => t('Use profile field for user name'),
      '#options' => array(0 => '- None -') + $choices,
      '#default_value' => variable_get('shoutbox_profile_name', 0),
      '#description' => t('Select an existing custom profile field to use as the user name or select - None - for the regular username.'),
    );
  }
  else {
    $form['display_settings']['shoutbox_profile_name'] = array(
      '#type' => 'item',
      '#title' => t('Use custom field for user name'),
      '#description' => t('Add custom field in <a href="@page_url">Account Setting</a> or enable the profile module to allow
                            Shoutbox to use different field instead of the username.', array('@page_url' => '../people/accounts/fields')),
    );
  }

  $form['display_settings']['shoutbox_replace_shout'] = array(
    '#type' => 'item',
    '#title' => t('Change "shout" globally'),
    '#description' => t('Set replacement for "shout". Does not apply to admin section. (Please use lowercase letters e.g. shout,shouts)'),
  );

  $form['display_settings']['shoutbox_replace_shout']['shoutbox_replace_singular'] = array(
    '#type' => 'textfield',
    '#title' => t('Singular'),
    '#default_value' => variable_get('shoutbox_replace_singular', 'shout'),
    '#size' => 15,
    '#required' => FALSE,
    '#attributes' => array('class' => array('shoutbox_replace')),
  );

  $form['display_settings']['shoutbox_replace_shout']['shoutbox_replace_plural'] = array(
    '#type' => 'textfield',
    '#title' => t('Plural'),
    '#default_value' => variable_get('shoutbox_replace_plural', 'Shout'),
    '#size' => 15,
    '#required' => FALSE,
    '#attributes' => array('class' => array('shoutbox_replace')),
  );
  /***end - display_settings****/

  /***start - filter_format****/
  _shoutbox_filter_form();
  $form['shoutbox_filter_format'] = array(
    '#type' => 'fieldset',
    '#title' => t('Shoutbox input format'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['shoutbox_filter_format']['shoutbox_escape_html'] = array(
    '#type' => 'checkbox',
    '#title' => t('Escape all HTML & formatting'),
    '#default_value' => variable_get('shoutbox_escape_html', 1),
    '#description' => '<strong>' . t('Checking this will ignore the input format selected below, and escape all HTML from the shouts.') . '</strong>'
  );

  $form['shoutbox_filter_format']['shoutbox_max_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum amount of characters in a shout'),
    '#default_value' => variable_get('shoutbox_max_length', 255),
    '#size' => 5,
    '#required' => TRUE,
    '#description' => t('Set the amount of allowed characters per shout. Enter 0 for no limit. This will not affect existing shouts.'),
  );


  $form['shoutbox_filter_format']['shoutbox_submit_type'] = array(
    '#type' => 'radios',
    '#title' => t('Sumbission type'),
    '#default_value' => variable_get('shoutbox_submit_type', 'post'),
    '#description' => t('Choose the type of submission used for a shout. <br/><small>Please note that "Ajax" submission does not cooperate(yet) with captcha module</small>'),
    '#options' => array(
      'ajax' => t('Ajax'),
      'post' => t('Post'),
    ),
  );
  /***end - filter_format****/

  /***start - time_settings****/
  $form['time_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Time settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['time_settings']['shoutbox_refresh'] = array(
    '#type' => 'textfield',
    '#title' => t('Auto refresh (in seconds)'),
    '#default_value' => shoutbox_get_refresh_rate(),
    '#size' => 4,
    '#maxlength' => 4,
    '#description' => t('Shoutbox can be set to automatically refresh every x number of seconds.  Set to 0 to turn off the auto refresh.'),
  );
  $form['time_settings']['shoutbox_anonymous_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of minutes for which anonymous users may edit or delete their own posts'),
    '#default_value' => variable_get('shoutbox_anonymous_timeout', 20),
    '#size' => 4,
    '#maxlength' => 4,
    '#description' => t('Anonymous users can edit or delete their post within this amount of time from it being posted, as long as they have the same IP address as when they posted it.  If you don\'t want shout editing and/or deleting, remove these permissions from Drupal\'s anonymous users role. Set 0 for no limit.'),
  );
  $form['time_settings']['shoutbox_registered_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of minutes for which registered users may edit or delete their own posts'),
    '#default_value' => variable_get('shoutbox_registered_timeout', 0),
    '#size' => 4,
    '#maxlength' => 4,
    '#description' => t('Registered users can edit or delete their post within this amount of time from it being posted.  If you don\'t want editing and/or deleting, remove these permissions from Drupal\'s authenticated users role. Set 0 for no limit.'),
  );
  $form['time_settings']['shoutbox_expire'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of days after which shouts will be purged from the database'),
    '#default_value' => variable_get('shoutbox_expire', 0),
    '#size' => 4,
    '#maxlength' => 4,
    '#description' => t('Shouts will be permanently deleted after the number of days specified.  Shouts will never expire when this is set to 0.'),
  );
  /***start - time_settings****/

  return system_settings_form($form);
}

/**
 * Validate the admin settings form.
 */
function shoutbox_admin_settings_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['shoutbox_showamount_block'])) {
    form_set_error('shoutbox_showamount_block', t('Block amount must be numeric'));
  }

  if (!is_numeric($form_state['values']['shoutbox_showamount_page'])) {
    form_set_error('shoutbox_showamount_page', t('Page amount must be numeric'));
  }

  if (!is_numeric($form_state['values']['shoutbox_max_length'])) {
    form_set_error('shoutbox_max_length', t('Shoutbox length must be numeric'));
  }

  if (!is_numeric($form_state['values']['shoutbox_refresh'])) {
    form_set_error('shoutbox_refresh', t('Refresh setting must be a numeric value'));
  }

  if (!is_numeric($form_state['values']['shoutbox_anonymous_timeout'])) {
    form_set_error('shoutbox_anonymous_timeout', t('Anonymous timeout setting must be a numeric value'));
  }

  if (!is_numeric($form_state['values']['shoutbox_registered_timeout'])) {
    form_set_error('shoutbox_registered_timeout', t('Registered timeout setting must be a numeric value'));
  }

  if (!is_numeric($form_state['values']['shoutbox_expire'])) {
    form_set_error('shoutbox_expire', t('Expiration setting must be a numeric value'));
  }

  if ($form_state['values']['shoutbox_replace_singular'] != '' || $form_state['values']['shoutbox_replace_singular'] != '') {
    {

      if ($form_state['values']['shoutbox_replace_singular'] == '') {
        form_set_error('shoutbox_replace_singular', t('Both fields must be empty or filled.'));
      }
      elseif ($form_state['values']['shoutbox_replace_plural'] == '') {
        form_set_error('shoutbox_replace_plural', t('Both fields must be empty or filled.'));
      }
      else {

        $bProceed = TRUE;
        if (is_numeric($form_state['values']['shoutbox_replace_singular'])) {
          form_set_error('shoutbox_replace_singular', t('"shout" replacement must contain only letters.'));
        }

        if (is_numeric($form_state['values']['shoutbox_replace_plural'])) {
          form_set_error('shoutbox_replace_plural', t('"shout" replacement must contain only letters.'));
        }

        if ($bProceed) {
          if (isset($form_state['values']['shoutbox_replace_singular']) && $form_state['values']['shoutbox_replace_singular'] != '') {
            $form_state['values']['shoutbox_replace_singular'] = strtolower($form_state['values']['shoutbox_replace_singular']);
          }

          if (isset($form_state['values']['shoutbox_replace_plural']) && $form_state['values']['shoutbox_replace_plural'] != '') {
            $form_state['values']['shoutbox_replace_plural'] = strtolower($form_state['values']['shoutbox_replace_plural']);
          }
        }
      }
    }
  }
}

/**
 * Shout delete confirmation form.
 *
 * @param shout_id
 *   The shout id of the shout being edited.
 */
function shoutbox_delete_form($form, &$form_state, $shout) {
  theme('shoutbox_external_files');
  _shoutbox_sanitize_shout($shout);

  $form['shout_preview'] = array(
    '#type' => 'item',
    '#value' => theme('shoutbox_post', array('shout' => $shout)),
  );
  $form['shout_id'] = array(
    '#type' => 'value',
    '#value' => $shout->shout_id,
  );
  $path = drupal_get_destination();
  $path = $path['destination'];
  $form = confirm_form($form, t('Are you sure you want to delete this ' . DEFAULTSHOUTSINGULAR . '?'), $path);
  return $form;
}

/**
 * Handle the delete form submission.
 */
function shoutbox_delete_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Confirm')) {
    if (is_numeric($form_state['values']['shout_id'])) {
      // Load the shout.
      $shout = shoutbox_shout_load($form_state['values']['shout_id']);

      // Delete the shout.
      shoutbox_delete_shout($shout);

      drupal_set_message(t('Your ' . DEFAULTSHOUTSINGULAR . ' was deleted.'));
      $form_state['redirect'] = '';
    }
    else {
      return drupal_not_found();
    }
  }
}

/**
 * Shout publish confirmation form.
 *
 * @param shout_id
 *   The shout id of the shout being published.
 */
function shoutbox_publish_form($form, &$form_state, $shout) {
  theme('shoutbox_external_files');
  _shoutbox_sanitize_shout($shout);

  $form['shout_preview'] = array(
    '#type' => 'item',
    '#value' => theme('shoutbox_post', array('shout' => $shout)),
  );
  $form['shout_id'] = array(
    '#type' => 'value',
    '#value' => $shout->shout_id,
  );
  $path = array_key_exists('destination', $_GET) ? $_GET['destination'] : '';
  $form = confirm_form($form, t('Are you sure you want to publish this ' . DEFAULTSHOUTSINGULAR . '?'), $path);
  return $form;
}

/**
 * Handle the publish form submission.
 */
function shoutbox_publish_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Confirm')) {
    $form_state['redirect'] = '';
    return shoutbox_moderate_shout($form_state['values']['shout_id'], FALSE);
  }
}

/**
 * Shout unpublish confirmation form.
 *
 * @param shout_id
 *   The shout id of the shout being unpublished.
 */
function shoutbox_unpublish_form($form, &$form_state, $shout) {
  theme('shoutbox_external_files');
  _shoutbox_sanitize_shout($shout);

  $form['shout_preview'] = array(
    '#type' => 'item',
    '#value' => theme('shoutbox_post', array('shout' => $shout)),
  );
  $form['shout_id'] = array(
    '#type' => 'value',
    '#value' => $shout->shout_id,
  );

  $form = confirm_form($form, t('Are you sure you want to unpublish this ' . DEFAULTSHOUTSINGULAR . '?'), $_GET['destination']);
  return $form;
}

/**
 * Handle the unpublish form submission.
 */
function shoutbox_unpublish_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Confirm')) {
    $form_state['redirect'] = '';
    return shoutbox_moderate_shout($form_state['values']['shout_id'], TRUE);
  }
}

/**
 * Form for editing shouts.
 *
 * @param shout_id
 *   The shout id of the shout being edited.
 */
function shoutbox_edit_form($form, &$form_state, $shout) {
  global $user;

  $form[] = array(
    '#type' => 'item',
    '#title' => t('Author'),
    '#markup' => l($shout->nick, 'user/' . $shout->uid),
  );
  $form[] = array(
    '#type' => 'item',
    '#title' => t('Created'),
    '#markup' => date('m/d/y h:i:sa', $shout->created),
  );
  $form[] = array(
    '#type' => 'item',
    '#title' => t('Changed'),
    '#markup' => date('m/d/y h:i:sa', $shout->changed),
  );

  if (_shoutbox_user_access('administer shoutbox') || _shoutbox_user_access('moderate shoutbox')) {
    $form['moderate'] = array(
      '#type' => 'radios',
      '#title' => t('Moderation status'),
      '#default_value' => $shout->moderate,
      '#options' => array('published', 'not published'),
    );
  }

  if (_shoutbox_user_access('edit own shouts', $shout)) {
    $max = variable_get('shoutbox_max_length', 255);
    $form['shout'] = array(
      '#type' => variable_get('shoutbox_widget_type', 'textfield'),
      '#title' => t('Shout'),
      '#default_value' => $shout->shout,
      '#cols' => 13,
      '#required' => TRUE,
      '#rows' => 7,
      '#maxlength' => $max ? $max : NULL,
    );

    $form['shout_id'] = array(
      '#type' => 'value',
      '#value' => $shout->shout_id,
    );
  }
  $path = drupal_get_destination();
  $path = $path['destination'];
  $form = confirm_form($form, '', $path, '', t('Update'), t('Cancel'));
  return $form;
}

/**
 * Handle the edit form submission.
 *
 * @param $form_id
 *   The form ID of the form.
 * @param $form_values
 *   Form values.
 */
function shoutbox_edit_form_submit($form, &$form_state) {
  // if (is_numeric($form_state['values']['shout_id'])) {
  // Load the existing shout if shout id is valid.
  $shout = shoutbox_shout_load($form_state['values']['shout_id']);
  if ($shout) {
    // Edit the shout object.
    $shout->shout = $form_state['values']['shout'];
    if (isset($form_state['values']['moderate'])) {
      $shout->moderate = $form_state['values']['moderate'];
    }
    $shout->changed = REQUEST_TIME;

    // Alert other modules about the update.
    $a1 = NULL;
    shoutbox_invoke('edit', $shout, $a1, $form_state);

    // Update the shout.
    // TODO Please review the conversion of this statement to the D7 database API syntax.
    /* db_query("UPDATE {shoutbox} SET shout = '%s', moderate = %d, changed = %d WHERE shout_id = %d", $shout->shout, $shout->moderate, $shout->changed, $shout->shout_id) */

    db_update('shoutbox')
      ->fields(array(
        'shout' => $shout->shout,
        'moderate' => $shout->moderate,
        'changed' => $shout->changed,
      ))
      ->condition('shout_id', $shout->shout_id)
      ->execute();

    drupal_set_message(t('Your ' . DEFAULTSHOUTSINGULAR . ' has been saved.'));
    $form_state['redirect'] = '';
  }
  else {
    return drupal_not_found();
  }
}
