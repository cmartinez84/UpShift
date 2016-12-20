<?php
/**
 * @file
 * Defines the Entity API Views controller class for group memberships.
 */

/**
 * Controller for generating Views data for Group memberships.
 */
class GroupMembershipViewsController extends EntityDefaultViewsController {

  /**
   * Add group entity relationships to groups.
   */
  public function views_data() {
    $data = parent::views_data();

    $data['group_membership']['role'] = array(
      'title' => t('Group role'),
      'help' => t('The granted group roles for a membership.'),
      'real field' => 'mid',
      // We do not add this field onto the {group_membership_role} table,
      // because the handler only requires the selected mids which it then
      // uses in a subquery to avoid duplicate rows. For more info, read the
      // handler's class documentation.
      'field' => array(
        'handler' => 'group_handler_field_membership_role',
        'no group by' => TRUE,
      ),
      'relationship' => array(
        'handler' => 'group_handler_relationship_membership_role',
        'label' => t('Membership role'),
        'base' => 'group_role',
      ),
    );

    // Define the many-to-many table so we can filter on group roles without
    // having to create a relationship first.
    $data['group_membership_role']['table'] = array(
      'group' => t('Group membership'),
      'join' => array(
        'group_membership' => array(
          'left_field' => 'mid',
          'field' => 'mid',
        ),
      ),
    );

    $data['group_membership_role']['role'] = array(
      'title' => t('Group role'),
      'help' => t('The granted group roles for a membership.'),
      'filter' => array(
        'handler' => 'group_handler_filter_membership_role',
        'allow empty' => TRUE,
      ),
    );

    return $data;
  }

}
