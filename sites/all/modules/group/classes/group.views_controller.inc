<?php
/**
 * @file
 * Defines the Entity API Views controller class for groups.
 */

/**
 * Controller for generating Views data for Group.
 */
class GroupViewsController extends EntityDefaultViewsController {

  /**
   * Add group entity relationships to groups.
   */
  public function views_data() {
    $data = parent::views_data();

    // Loop over all group entities.
    foreach (entity_get_info() as $type => $e_info) {
      if (!empty($e_info['group entity'])) {
        // Add a child count field to all group entities.
        $data['groups']["group_entity_{$type}_count"] = array(
          'title' => t('Count @label entities', array('@label' => $e_info['label'])),
          'help' => t('The amount of @label entities that belong to a group.<br />Only use this field when you need <strong>multiple child entity counts</strong> for a <strong>single Group table</strong>.', array('@label' => $e_info['label'])),
          'field' => array(
            'handler' => 'group_handler_field_child_entity_count',
            'click sortable' => TRUE,
            'entity type' => $type,
          ),
        );

        // Add a child entity relationship to all group entities.
        $data['groups']["group_entity_$type"] = array(
          'title' => t('@label entities', array('@label' => $e_info['label'])),
          'help' => t('@label entities that belong to a group.', array('@label' => $e_info['label'])),
          'relationship' => array(
            'handler' => 'group_handler_relationship_child_entity',
            'label' => t('Group @label', array('@label' => $e_info['label'])),
            'base' => $e_info['base table'],
            'entity type' => $type,
          ),
        );

        // Add a parent group relationship to all group entities.
        $data[$e_info['base table']]['parent_group'] = array(
          'title' => t('Parent group'),
          'help' => t('The group an entity belongs to.'),
          'relationship' => array(
            'handler' => 'group_handler_relationship_parent_group',
            'label' => t('@label group', array('@label' => $e_info['label'])),
            'base' => 'groups',
            'entity type' => $type,
          ),
        );
      }
    }

    return $data;
  }

}
