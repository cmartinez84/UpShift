<?php
/**
 * @file
 * Defines the Entity API extra fields class for group memberships.
 */

/**
 * Controller for generating extra fields for group memberships.
 */
class GroupMembershipExtraFieldsController extends EntityDefaultExtraFieldsController {

  /**
   * Implements EntityExtraFieldsControllerInterface::fieldExtraFields().
   */
  public function fieldExtraFields() {
    $extra = array();

    $extra['group_membership']['group_membership']['form']['roles'] = array(
      'label' => t('Group roles'),
      'description' => t('The roles the member has, will only show when you have access.'),
      'weight' => 20,
    );

    // @todo Loop over all profiles and add them here?

    return $extra;
  }
}
