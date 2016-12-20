<?php
/**
 * @file
 * Defines the Entity API UI class for group types.
 */

/**
 * UI class for group types.
 */
class GroupTypeUIController extends EntityDefaultUIController {

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = -5;

    $items["$this->path/manage/%entity_object/edit"]['weight'] = -10;

    $items["$this->path/manage/%group_type/permissions"] = array(
      'title' => 'Permissions',
      'description' => 'Edit permissions for groups of this type',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_permission_form', 4),
      'access callback' => 'user_access',
      'access arguments' => array('configure group module'),
      'file' => 'admin/group.permission.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 0,
    );

    $items["$this->path/manage/%group_type/permissions/list"] = array(
      'title' => 'Permissions',
      'description' => 'Edit permissions for this group type',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => 0,
    );

    $items["$this->path/manage/%group_type/permissions/roles"] = array(
      'title' => 'Roles',
      'description' => 'Manage group roles for this group type.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_role_overview_form', 'group_role', 4),
      'access callback' => 'user_access',
      'access arguments' => array('configure group module'),
      'file' => 'includes/entity.ui.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 5,
    );

    $items["$this->path/manage/%group_type/permissions/roles/add"] = array(
      'title' => 'Add group role',
      'page callback' => 'group_type_add_role_form',
      'page arguments' => array(4),
      'access callback' => 'group_type_add_role_access',
      'access arguments' => array(4),
      'file' => 'admin/group_role.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_ACTION,
      'weight' => 0,
    );

    $items["$this->path/manage/%group_type/config"] = array(
      'title' => 'Config',
      'description' => 'Configure the group type.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_type_config_form', 4),
      'access callback' => 'user_access',
      'access arguments' => array('configure group module'),
      'file' => 'admin/group_type.config.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => -5,
    );

    $items["$this->path/manage/%group_type/config/owner"] = array(
      'title' => 'Group owner',
      'description' => 'Configuration of the creating member.',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    return $items;
  }

  /**
   * Builds the entity overview form.
   */
  public function overviewForm($form, &$form_state) {
    $form['table'] = $this->typeTable();
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save order'),
    );

    return $form;
  }

  /**
   * Overview form submit callback.
   *
   * @param array $form
   *   The form array of the overview form.
   * @param array $form_state
   *   The overview form state which will be used for submitting.
   */
  public function overviewFormSubmit($form, &$form_state) {
    if (empty($form_state['values']['weights'])) {
      return;
    }

    foreach ($form_state['values']['weights'] as $name => $weight) {
      $group_type = group_type_load($name);

      if (entity_has_status('group_type', $group_type, ENTITY_FIXED)) {
        continue;
      }

      if ($group_type->weight != $weight) {
        $group_type->weight = $weight;
        $group_type->save();
      }
    }
  }

  /**
   * Builds the operation form.
   */
  public function operationForm($form, &$form_state, $group_type, $op) {
    if ($op == 'delete') {
      $form['info'] = array(
        '#markup' => t('By deleting this group type you will delete all groups of that type, including their content.'),
        '#suffix' => '<br />',
      );

      $message = 'Are you sure you want to delete the group type %label and all of its groups?';
      $replace = array(
        '%label' => entity_label('group_type', $group_type),
      );

      return confirm_form($form, t($message, $replace), $this->path, NULL, t('Delete'));
    }

    // Call parent method for other operations.
    return parent::operationForm($form, $form_state, $group_type, $op);
  }

  /**
   * Generates the render array for an overview table for group types.
   *
   * Builds a tabledrag without having to write a theme function for it.
   * See http://dropbucket.org/node/204.
   *
   * @return array
   *   A renderable array.
   */
  protected function typeTable() {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'group_type');
    $query->propertyOrderBy('weight');

    if ($this->overviewPagerLimit) {
      $query->pager($this->overviewPagerLimit);
    }

    $result = $query->execute();

    $rows = $weights = array();
    if (isset($result['group_type'])) {
      $group_types = group_types(array_keys($result['group_type']));

      foreach ($group_types as $name => $group_type) {
        $row = $this->typeTableRow($name, $group_type);
        $rows[$name] = $row;

        $weights[$name] = array(
          'weight' => &$rows[$name]['data']['weight']['data'],
        );
      }
    }

    $table = array(
      '#theme' => 'table',
      '#header' => $this->typeTableHeaders(),
      '#rows' => $rows,
      '#empty' => t('There are no group types available.'),
      '#attributes' => array('id' => 'group-type-table'),
      'elements' => $weights,
    );

    drupal_add_tabledrag('group-type-table', 'order', 'sibling', 'group-type-weight');

    return $table;
  }

  /**
   * Returns the available operations for the page.
   */
  protected function typeOperations() {
    $ops = array('edit', 'config');
    if (module_exists('field_ui')) {
      array_push($ops, 'manage fields', 'manage display');
    }
    if (module_exists('i18n_string')) {
      $ops[] = 'translate';
    }
    array_push($ops, 'clone', 'delete', 'export');

    return $ops;
  }

  /**
   * Generates the table headers for the type table.
   */
  protected function typeTableHeaders() {
    $header = array(
      'drag' => array(
        'data' => NULL,
        'colspan' => 2,
      ),
      'label' => t('Name'),
      'permissions' => array(
        'data' => t('Permissions'),
        'colspan' => 2,
      ),
      'status' => t('Status'),
    );

    // Add the actions column.
    $header['actions'] = array(
      'data' => t('Actions'),
      'colspan' => count($this->typeOperations()),
    );

    return $header;
  }

  /**
   * Generates the row for the passed type.
   */
  protected function typeTableRow($name, GroupType $group_type) {
    // Check if the group type is ENTITY_FIXED.
    $type_fixed = entity_has_status('group_type', $group_type, ENTITY_FIXED);

    // Create the row container array.
    $row = array(
      'data' => array(),
      'class' => array('draggable'),
    );

    // Cell for the cross drag&drop element.
    $row['data']['drag'] = array(
      'class' => array('entry-cross'),
    );

    // Weight item for the tabledrag.
    $row['data']['weight'] = array(
      'data' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#title_display' => 'invisible',
        '#default_value' => $group_type->weight,
        '#parents' => array('weights', $name),
        '#attributes' => array(
          'class' => array('group-type-weight'),
        ),
        '#disabled' => $type_fixed,
      ),
    );

    // Add the type label.
    $row['data']['label'] = $group_type->label();

    // Permissions related links.
    $what = $type_fixed ? t('view permissions') : t('edit permissions');
    $row['data']['permissions'] = l($what, "$this->path/manage/$name/permissions");
    $row['data']['roles'] = l(t('roles'), "$this->path/manage/$name/permissions/roles");

    // Add the status column.
    $row['data']['status'] = array(
      'data' => array(
        '#theme' => 'entity_status',
        '#status' => $group_type->status,
      ),
    );

    // Add the edit action.
    $row['data']['edit'] = $type_fixed ? '' : l(t('edit'), "$this->path/manage/$name");

    // Add the config action.
    $row['data']['config'] = $type_fixed ? '' : l(t('config'), "$this->path/manage/$name/config");

    // Add the Field UI actions.
    if (module_exists('field_ui')) {
      $row['data']['manage fields'] = l(t('manage fields'), "$this->path/manage/$name/fields");
      $row['data']['manage display'] = l(t('manage display'), "$this->path/manage/$name/display");
    }

    // Add the Group Translation action.
    if (module_exists('i18n_string')) {
      $row['data']['translate'] = l(t('translate'), "$this->path/manage/$name/translate");
    }

    // Add the clone action.
    $row['data']['clone'] = l(t('clone'), "$this->path/manage/$name/clone");

    // Don't show delete action for fixed or default types.
    if ($type_fixed || $group_type->status == ENTITY_IN_CODE) {
      $row['data']['delete'] = '';
    }

    // Add the delete/revert action.
    else {
      // Show revert action for overridden roles.
      if (entity_has_status('group_type', $group_type, ENTITY_OVERRIDDEN)) {
        $what = t('revert');
        $path = "$this->path/manage/$name/revert";
      }

      // Show delete action for custom types.
      else {
        $what = t('delete');
        $path = "$this->path/manage/$name/delete";
      }

      $row['data']['delete'] = l($what, $path, array('query' => drupal_get_destination()));
    }

    // Add the export action.
    $row['data']['export'] = l(t('export'), "$this->path/manage/$name/export");

    return $row;
  }
}
