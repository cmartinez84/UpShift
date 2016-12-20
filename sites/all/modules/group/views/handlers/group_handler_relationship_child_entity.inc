<?php
/**
 * @file
 * Definition of group_handler_relationship_child_entity.
 */

/**
 * Handler to retrieve group entities for a group.
 *
 * @ingroup views_relationship_handlers
 */
class group_handler_relationship_child_entity extends views_handler_relationship {

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    $this->ensure_my_table();

    // First we join {groups} with {group_entity}.
    $join_definition = array(
      'left_table' => $this->table_alias,
      'left_field' => 'gid',
      'table' => 'group_entity',
      'field' => 'gid',
    );

    if (!empty($this->options['required'])) {
      $join_definition['type'] = 'INNER';
    }

    $join = new views_join();
    $join->definition = $join_definition;
    $join->construct();
    $join->adjusted = TRUE;

    $this->group_entity_alias = $this->query->add_table('group_entity', $this->relationship, $join);

    // Then we join {group_entity} with whatever was set in 'base'.
    $entity_table = $this->definition['base'];
    $table_data = views_fetch_data($entity_table);

    $join_definition = array(
      'left_table' => $this->group_entity_alias,
      'left_field' => 'entity_id',
      'table' => $this->definition['base'],
      'field' => $table_data['table']['base']['field'],
      'extra' => array(
        array(
          'table' => $this->group_entity_alias,
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

    // Use a short alias for this.
    $alias = "group_entity_$entity_table";

    // Add the relationship to the view.
    $this->alias = $this->query->add_relationship($alias, $join, $entity_table, $this->relationship);
  }
}
