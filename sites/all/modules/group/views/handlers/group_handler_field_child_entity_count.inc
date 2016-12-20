<?php
/**
 * @file
 * Definition of group_handler_field_child_entity_count.
 */

/**
 * Handler to retrieve the amount of group entities for a group.
 *
 * Note that this functionality can be achieved as well by using Views's
 * aggregate functions ("Use aggregation" under Advanced). The key difference
 * is that this handler allows multiple count fields in one view.
 *
 * You should only use this 'field' on views with just a single occurrence of
 * the {groups} table, any higher amount will lead to unexpected results.
 *
 * @ingroup views_relationship_handlers
 */
class group_handler_field_child_entity_count extends views_handler_field {

  /**
   * Called to add the field to the query.
   */
  function query() {
    $this->ensure_my_table();

    // Construct a special alias for the join with {group_entity}. By using the
    // same table alias for the join, it won't happen more than once. See
    // views_plugin_query_default::add_table().
    $alias = "{$this->table_alias}_child_entity_count";

    // We join {groups} with {group_entity} using the special alias.
    $join_definition = array(
      'left_table' => $this->table_alias,
      'left_field' => 'gid',
      'table' => 'group_entity',
      'field' => 'gid',
    );

    $join = new views_join();
    $join->definition = $join_definition;
    $join->construct();
    $join->adjusted = TRUE;

    $this->query->add_table('group_entity', $this->relationship, $join, $alias);

    // We add a special SUM(CASE WHEN) expression to do the counting.
    $type = $this->definition['entity type'];
    $expression = "CASE WHEN $alias.entity_type = '$type' THEN 1 ELSE 0 END";
    $this->field_alias = $this->query->add_field(NULL, $expression, "group_entity_{$type}_count", array('function' => 'sum'));
  }
}
