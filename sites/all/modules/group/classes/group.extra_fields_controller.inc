<?php
/**
 * @file
 * Defines the Entity API extra fields class for groups.
 */

/**
 * Controller for generating extra fields for groups and group entities.
 */
class GroupExtraFieldsController extends EntityDefaultExtraFieldsController {

  /**
   * Implements EntityExtraFieldsControllerInterface::fieldExtraFields().
   */
  public function fieldExtraFields() {
    $extra = array();

    foreach (group_types() as $type => $group_type) {
      // Add the 'title' form field to every group type.
      $extra['group'][$type]['form']['title'] = array(
        'label' => t('Group title'),
        'description' => t('The name of the group'),
        'weight' => -5,
      );

      // Add the membership action form.
      $extra['group'][$type]['display']['membership_actions'] = array(
        'label' => t('Membership action form'),
        'description' => t('The form containing various membership actions such as: join, leave, request membership, ...'),
        'weight' => -4,
      );

      // Add Path module support.
      if (module_exists('path')) {
        $extra['group'][$type]['form']['path'] = array(
          'label' => t('URL path settings'),
          'description' => t('Path module form elements'),
          'weight' => 99,
        );
      }
    }

    return $extra;
  }
}
