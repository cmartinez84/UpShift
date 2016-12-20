<?php
/**
 * @file
 * Definition of group_handler_relationship_membership_role.
 */

/**
 * Handler to retrieve group roles for a membership.
 *
 * @ingroup views_relationship_handlers
 */
class group_handler_relationship_membership_role extends views_handler_relationship {

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    $this->ensure_my_table();

    // First we join {group_membership} with {group_membership_role}.
    $join_definition = array(
      'left_table' => $this->table_alias,
      'left_field' => 'mid',
      'table' => 'group_membership_role',
      'field' => 'mid',
    );

    if (!empty($this->options['required'])) {
      $join_definition['type'] = 'INNER';
    }

    $join = new views_join();
    $join->definition = $join_definition;
    $join->construct();
    $join->adjusted = TRUE;

    $this->gmr_alias = $this->query->add_table('group_membership_role', $this->relationship, $join);

    // Then we join {group_membership_role} with {group_role}.
    $join_definition = array(
      'left_table' => $this->gmr_alias,
      'left_field' => 'role',
      'table' => 'group_role',
      'field' => 'name',
    );

    if (!empty($this->options['required'])) {
      $join_definition['type'] = 'INNER';
    }

    $join = new views_join();
    $join->definition = $join_definition;
    $join->construct();
    $join->adjusted = TRUE;

    // Use a short alias for this.
    $alias = "{$this->table_alias}_roles";

    // Add the relationship to the view.
    $this->alias = $this->query->add_relationship($alias, $join, 'group_role', $this->relationship);
  }
}
