<?php
/**
 * @file
 * Defines the Features integration class for group roles.
 */

/**
 * Main class for group role Features integration.
 */
class GroupRoleFeaturesController extends EntityDefaultFeaturesController {

  /**
   * Defines the result for hook_features_api().
   */
  public function api() {
    $return = parent::api();
    $return['group_role']['name'] = t('Group roles (global), including permissions');

    return $return;
  }

  /**
   * Generates the result for hook_features_export_options().
   */
  public function export_options() {
    $options = array();

    foreach (entity_load_multiple_by_name('group_role', FALSE) as $name => $group_role) {
      if ($group_role->global) {
        $options[$name] = $group_role->label();
      }
    }

    return $options;
  }
}
