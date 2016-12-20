<?php
/**
 * @file
 * Defines the Features integration class for group types.
 */

/**
 * Main class for group type Features integration.
 */
class GroupTypeFeaturesController extends EntityDefaultFeaturesController {

  /**
   * Defines the result for hook_features_api().
   */
  public function api() {
    $return = parent::api();
    $return['group_type']['name'] = t('Group types, including roles and permissions');

    return $return;
  }
}
