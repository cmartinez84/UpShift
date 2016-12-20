<?php
/**
 * @file
 * Definition of group_handler_filter_membership_roles.
 */

/**
 * Filter handler for group membership roles.
 *
 * @ingroup views_filter_handlers
 */
class group_handler_filter_membership_role extends views_handler_filter_many_to_one {
  function get_value_options() {
    $this->value_options = group_role_labels();
  }

  /**
   * Override empty and not empty operator labels to be clearer for membership
   * roles.
   */
  function operators() {
    $operators = parent::operators();
    $operators['empty']['title'] = t("Only has the 'member' role");
    $operators['not empty']['title'] = t("Has roles in addition to 'member'");
    return $operators;
  }
}
