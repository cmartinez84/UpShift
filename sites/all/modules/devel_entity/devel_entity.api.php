<?php

/**
 * hook_devel_entity_info().
 * @TODO: Write document for this.
 */
function hook_devel_entity_info() {
  $info = array();  
  foreach (profile2_get_types() as $type_name => $type) {
    if (!empty($type->data['use_page'])) {
      $path = profile2_page_get_base_path($type) . '/%profile2_by_uid';
      $info['profile2:' . $type_name] = array(
        'entity type' => 'profile2',
        'view path' => $path,
        'label' => 'profile',
      );
    }
  }

  return $info;
}

/**
 * hook_devel_entity_info_alter().
 * @TODO: Write document for this.
 */
function hook_devel_entity_info_alter(&$devel_entity_info) {
	// Alter entity info.
}
