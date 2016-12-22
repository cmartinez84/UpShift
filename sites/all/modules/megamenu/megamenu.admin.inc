<?php

/**
 * Implements hook_form().
 */
function megamenu_admin_form($form, &$form_state = NULL) {
  $form = array();

  $form['markup'] = array(
    '#markup' => t('To enable a Megamenu, go to !link in the admin section. There you can place a Megamenu, most likely in the header region, and it will inherit its structure from the associated Drupal menu.', array('!link' => l(t('Site building -> Blocks'), 'admin/structure/block'))),
  );

  foreach (_megamenu_menulist() as $menu) {
    $menu_details = menu_load($menu);
    _megamenu_verify_menu_entry($menu_details['menu_name']);
    $enabled = _megamenu_is_enabled($menu_details['menu_name']);

    $form['enabled'][$menu_details['menu_name']] = array(
      '#title' => t('Enabled'),
      '#type' => 'checkbox',
      '#default_value' => $enabled ? $enabled : 0,
    );
  }

  $form['save_configuration'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['advanced'] = array(
    '#title' => 'Advanced Settings',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#type' => 'fieldset',
    '#description' => t('Default menu timeout (applied to all menus).'),
  );
  $form['advanced']['timeout'] = array(
    '#title' => t('Menu Timeout'),
    '#type' => 'textfield',
    '#default_value' => variable_get('megamenu_menu_timeout', 500),
    '#description' => t('Menu timeout, default is 500.'),
  );
  $form['advanced']['sizewait'] = array(
    '#title' => t('Menu Size Wait'),
    '#type' => 'textfield',
    '#default_value' => variable_get('megamenu_menu_sizewait', 250),
    '#description' => t('Menu size wait, default is 250.'),
  );
  $form['advanced']['hoverwait'] = array(
    '#title' => t('Menu Hover Wait'),
    '#type' => 'textfield',
    '#default_value' => variable_get('megamenu_menu_hoverwait', 400),
    '#description' => t('Menu hover wait, default is 400.'),
  );
  $form['#theme'] = 'megamenu_admin';
  $form['#validate'][] = 'megamenu_admin_form_validate';
  $form['#submit'][] = 'megamenu_admin_form_submit';

  return $form;
}

function megamenu_settings_form($form, &$form_state, $menu_name = '') {
  drupal_set_title(t('Megamenu settings for: @menu-name', array('@menu-name' => $menu_name)));

  $form = array();

  $form['orientations'] = array(
    '#title' => t('Orientations'),
    '#type' => 'fieldset',
    '#description' => t('Set the orientation of various menu elements.'),
  );

  $form['orientations']['menu_orientation'] = array(
    '#title' => t('Menu Orientation'),
    '#type' => 'radios',
    '#options' => array('horizontal' => 'horizontal', 'vertical' => 'vertical'),
    '#default_value' => _megamenu_get_menu_orientation_by_name($menu_name),
    '#description' => t('Select whether the mega menu will extend horizontally and drop down, or if it will extend vertically and fly out.'),
  );

  $form['orientations']['slot_orientation'] = array(
    '#title' => t('Slot Orientation'),
    '#type' => 'radios',
    '#options' => array('columnar' => 'columnar', 'stacking' => 'stacking'),
    '#default_value' => _megamenu_get_slot_orientation_by_name($menu_name),
    '#description' => t('Select whether slots will sit next to each other (columnar) or stack on top of each other (stacking).'),
  );

  $form['style'] = array(
    '#title' => 'Style Settings',
    '#type' => 'fieldset',
    '#description' => t('Select a skin for this menu'),
  );

  // @TODO: Create a function to return a list of default skins.
  $form['style']['skin_options'] = array(
    '#title' => t('Skin Type'),
    '#type' => 'radios',
    '#options' => array('supplied_skin' => t('Use a skin supplied with this module'), 'custom_skin' => t('Use your own custom skin')),
    '#default_value' => _megamenu_is_skin_default($menu_name) ? 'supplied_skin' : 'custom_skin',
    '#description' => t('Select if you wish to use a pre-defined skin'),
  );

  $form['style']['default_skin'] = array(
    '#title' => t('Supplied Skin Name'),
    '#type' => 'select',
    '#options' => array('friendly' => 'friendly', 'minimal' => 'minimal'),
    '#default_value' => _megamenu_is_skin_default($menu_name) ? _megamenu_get_skin_by_name($menu_name) : 'minimal',
    '#description' => t('Select one of the supplied skins to use'),
  );
  // @TODO: make this path a variable somehow so it can be called more easily.
  // @TODO: make the "file exists/does not" text stand out.
  /*
  $form['style']['custom_css'] = array(
    '#type' => 'item',
    '#title' => t('Custom CSS File'),
    '#value' => '<p>'. t('You can add your own custom CSS for extending or creating skins at %csspath', array('%csspath'=> file_directory_path() .'/megamenu/megamenu-custom.css')) .'</p><p>'. (file_exists(file_directory_path() .'/megamenu/megamenu-custom.css') ? t('Custom CSS file exists') : t('Custom CSS file does not exist')),
  );
  */
  $form['style']['custom_skin'] = array(
    '#title' => t('Custom Skin Name'),
    '#type' => 'textfield',
    '#default_value' => _megamenu_is_skin_default($menu_name) ? '' : _megamenu_get_skin_by_name($menu_name),
    '#description' => t('Type in the name of your custom skin (This will become a class value applied to the menu: megamenu-skin-[skin name]).'),
  );

  $form['save_configuration'] = array(
    '#type' => 'submit',
    '#value' => t('Save Configuration'),
  );

  // Value placeholder.
  $form['menu_name'] = array(
    '#type' => 'value',
    '#value' => $menu_name,
  );

  return $form;
}

/**
 * Validate the main admin form.
 */
function megamenu_admin_form_validate($form, &$form_state) {
  //drupal_set_message(t('Validated'));

  // @TODO: Is there anything to validate on this form?
  
}

/**
 * Submit the main admin form.
 */
function megamenu_admin_form_submit($form, &$form_state) {
  drupal_set_message(t('Configuration Saved'));
  variable_set('megamenu_menu_timeout', $form_state['values']['timeout']);
  variable_set('megamenu_menu_sizewait', $form_state['values']['sizewait']);
  variable_set('megamenu_menu_hoverwait', $form_state['values']['hoverwait']);

  foreach (_megamenu_menulist() as $menu_name) {
    db_update('megamenu')
      ->fields(array(
        'enabled' => $form_state['values'][$menu_name],
      ))
      ->condition('menu_name', $menu_name)
      ->execute();
  }
}

/**
 * Validate the settings form.
 */
function megamenu_settings_form_validate($form, &$form_state) {
  // $settings = $form_state['values']['settings']; WTF IS THIS?
  // Need to validate the advanced value. Must be number and can't be a negative value.
  if ($form_state['values']['skin_options'] == 'custom_skin') {
    if ($form_state['values']['custom_skin'] == '') {
      form_set_error('custom_skin', t('If you want to use a custom skin, you must specify its name'));
    }
    else if (!check_plain($form_state['values']['custom_skin'])) {
      form_set_error('custom_skin', t('Value must be plain text'));
    }
    else {
      $skin = $form_state['values']['custom_skin'];
    }

  }
  else {
    $skin = $form_state['values']['default_skin'];
  }

  $form_state['megamenu']['skin'] = $skin;
  
}

/**
 * Submit the settings form.
 */
function megamenu_settings_form_submit($form, &$form_state) {
  $menu_name = $form_state['values']['menu_name'];
  $skin = $form_state['megamenu']['skin'];
  $menu_orientation = $form_state['values']['menu_orientation'];
  $slot_orientation = $form_state['values']['slot_orientation'];

  db_update('megamenu')
    ->fields(array(
      'menu_orientation' => $menu_orientation,
      'slot_orientation' => $slot_orientation,
      'skin' => $skin,
    ))
    ->condition('menu_name', $menu_name)
    ->execute();
}
