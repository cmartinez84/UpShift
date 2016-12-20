<?php
/**
 * @file
 * Definition of group_handler_relationship_parent_group.
 */

/**
 * Handler to retrieve the parent group(s) for group entities.
 *
 * @ingroup views_relationship_handlers
 */
class group_handler_relationship_parent_group extends views_handler_relationship {

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    $this->ensure_my_table();

    // First we join the entity table with {group_entity}.
    $table_data = views_fetch_data($this->table);
    $join_definition = array(
      'left_table' => $this->table_alias,
      'left_field' => $table_data['table']['base']['field'],
      'table' => 'group_entity',
      'field' => 'entity_id',
      'extra' => array(
        array(
          'field' => 'entity_type',
          'value' => $this->definition['entity type'],
        ),
      ),
    );

    if (!empty($this->options['required'])) {
      $join_definition['type'] = 'INNER';
    }

    $join = new views_join();
    $join->definition = $join_definition;
    $join->construct();
    $join->adjusted = TRUE;

    $this->group_entity_alias = $this->query->add_table('group_entity', $this->relationship, $join);

    // Then we join {group_entity} with {groups}.
    $table_data = views_fetch_data('groups');
    $join_definition = array(
      'left_table' => $this->group_entity_alias,
      'left_field' => 'gid',
      'table' => 'groups',
      'field' => 'gid',
    );

    if (!empty($this->options['required'])) {
      $join_definition['type'] = 'INNER';
    }

    $join = new views_join();
    $join->definition = $join_definition;
    $join->construct();
    $join->adjusted = TRUE;

    // Use a short alias for this.
    $alias = "{$this->table_alias}_parent_groups";

    // Add the relationship to the view.
    $this->alias = $this->query->add_relationship($alias, $join, 'groups', $this->relationship);
  }
}
