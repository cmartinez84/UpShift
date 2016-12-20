<?php
/**
 * @file
 * Defines the Entity API UI class for group roles.
 */

/**
 * UI class for group roles.
 */
class GroupRoleUIController extends EntityDefaultUIController {

  /**
   * Whether the UI is invoked for global or local roles.
   *
   * @var bool
   */
  protected $global_ui;

  /**
   * Whether or not to show the tabledrag UI.
   *
   * @var bool
   */
  protected $tabledrag;

  /**
   * More specifically, whether the parent group type is ENTITY_FIXED.
   *
   * @var bool
   */
  protected $type_fixed;

  /**
   * Class constructor.
   */
  public function __construct($entity_type, $entity_info) {
    parent::__construct($entity_type, $entity_info);

    // By default we show the global UI and tabledrag.
    $this->global_ui = TRUE;
    $this->tabledrag = TRUE;

    // We assume roles that were not imported along with a group type.
    $this->type_fixed = FALSE;
  }

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    $items[$this->path]['title'] = 'Group roles';
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = 0;

    $items["$this->path/manage/%group_role/permissions"] = array(
      'title' => 'Permissions',
      'description' => 'Edit permissions for this role',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_permission_form', NULL, 4),
      'access callback' => 'user_access',
      'access arguments' => array('configure group module'),
      'file' => 'admin/group.permission.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 0,
    );

    return $items;
  }

  /**
   * Builds the entity overview form.
   */
  public function overviewForm($form, &$form_state) {
    if (!empty($form_state['build_info']['args'][1])) {
      $group_type = $form_state['build_info']['args'][1];

      // We're not viewing the global UI.
      $this->global_ui = FALSE;

      // Whether the parent group type is fixed.
      if (entity_has_status('group_type', $group_type, ENTITY_FIXED)) {
        $this->type_fixed = TRUE;
      }

      // Show only the roles for this group type.
      $conditions = array('type' => $group_type->name);
    }
    else {
      // Show all global roles.
      $conditions = array('global' => 1);
    }

    // Determine whether or not we show the tabledrag.
    $this->tabledrag = $this->global_ui || !$this->type_fixed;

    // Show the appropriate description for the overview page.
    $help = !$this->global_ui
      ? t("<p>These are <strong>group type specific roles</strong>, meaning they only apply to this group type. You can create global group roles by visiting <a href='@url'>the 'Group roles' page</a>.</p>", array('@url' => url('admin/group/role')))
      : t("<p>These are <strong>global group roles</strong>, meaning they apply to every group of every group type. Use this if you don't want to create the same role over and over again for every group type. A good usage example would be a <em>group moderator</em> role that has limited admin rights.</p><p>You can create group type specific roles under 'Roles' on the 'Permissions' tab of a group type.</p>");

    // Apply the variables set above.
    $form['help'] = array('#markup' => $help);
    $form['table'] = $this->roleTable($conditions);

    if ($this->tabledrag) {
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save order'),
      );
    }

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
      $group_role = group_role_load($name);

      if (entity_has_status('group_role', $group_role, ENTITY_FIXED)) {
        continue;
      }

      if ($group_role->weight != $weight) {
        $group_role->weight = $weight;
        $group_role->save();
      }
    }
  }

  /**
   * Generate an overview table for group roles matching the given conditions.
   *
   * Builds a tabledrag without having to write a theme function for it.
   * See http://dropbucket.org/node/204.
   *
   * @param array $conditions
   *   An array of conditions as needed by entity_load().
   *
   * @return array
   *   A renderable array.
   */
  protected function roleTable(array $conditions = array()) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'group_role');
    $query->propertyOrderBy('weight');

    // Add all conditions to query.
    foreach ($conditions as $key => $value) {
      $query->propertyCondition($key, $value);
    }

    if ($this->overviewPagerLimit) {
      $query->pager($this->overviewPagerLimit);
    }

    $result = $query->execute();

    $rows = $weights = array();
    if (isset($result['group_role'])) {
      $group_roles = group_roles(array_keys($result['group_role']));

      foreach ($group_roles as $name => $group_role) {
        $row = $this->roleTableRow($name, $group_role);
        $rows[$name] = $row;

        if ($this->tabledrag) {
          $weights[$name] = array(
            'weight' => &$rows[$name]['data']['weight']['data'],
          );
        }
      }
    }

    $table = array(
      '#theme' => 'table',
      '#header' => $this->roleTableHeaders(),
      '#rows' => $rows,
      '#empty' => t('There are no group roles available.'),
    );

    if ($this->tabledrag) {
      $table['elements'] = $weights;
      $table['#attributes'] = array('id' => 'group-role-table');

      drupal_add_tabledrag('group-role-table', 'order', 'sibling', 'group-role-weight');
    }

    return $table;
  }

  /**
   * Returns the available operations for the page.
   */
  protected function roleOperations() {
    $ops = array();

    // Entity status and export column for global roles.
    if ($this->global_ui) {
      $ops[] = 'status';

      if (!empty($this->entityInfo['exportable'])) {
        $ops[] = 'export';
      }
    }

    // Edit, clone and delete/revert links only show when we are viewing the
    // global UI or the roles for a non-fixed group type.
    if ($this->tabledrag) {
      array_push($ops, 'edit', 'clone', 'delete');
    }

    // Translate links should always be available, regardless of context or
    // whether or not the entity has a fixed status.
    if (module_exists('i18n_string')) {
      $ops[] = 'translate';
    }

    return $ops;
  }

  /**
   * Generates the table headers for the role table.
   */
  protected function roleTableHeaders() {
    $header = array();

    // Optionally add the tabledrag spacer.
    if ($this->tabledrag) {
      $header['drag'] = array(
        'data' => NULL,
        'colspan' => 2,
      );
    }

    $header['label'] = t('Name');
    $header['permissions'] = t('Permissions');

    // Add Entity API related columns.
    if ($ops = $this->roleOperations()) {
      $count = count($ops);

      // Add the status column if available.
      if (in_array('status', $ops)) {
        $header['status'] = t('Status');
        $count--;
      }

      // Add the actions column if available.
      if ($count) {
        $header['actions'] = array(
          'data' => t('Actions'),
          'colspan' => $count,
        );
      }
    }

    return $header;
  }

  /**
   * Generates the row for the passed role.
   */
  protected function roleTableRow($name, GroupRole $group_role) {
    // Check if the group role is ENTITY_FIXED.
    $role_fixed = entity_has_status('group_role', $group_role, ENTITY_FIXED);

    // Create the row container array.
    $row = array(
      'data' => array(),
      'class' => array('draggable'),
    );

    if ($this->tabledrag) {
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
          '#default_value' => $group_role->weight,
          '#parents' => array('weights', $name),
          '#attributes' => array(
            'class' => array('group-role-weight'),
          ),
          '#disabled' => $role_fixed,
        ),
      );
    }

    // Add the role label.
    $row['data']['label'] = $group_role->label();

    // Permissions related links.
    $what = $role_fixed || $this->type_fixed ? t('view permissions') : t('edit permissions');
    $row['data']['permissions'] = l($what, "$this->path/manage/$name/permissions");

    // Add the actions columns.
    if ($ops = $this->roleOperations()) {
      // Add the status column if available.
      if (in_array('status', $ops)) {
        $row['data']['status'] = array(
          'data' => array(
            '#theme' => 'entity_status',
            '#status' => $group_role->status,
          ),
        );
      }

      // Add the edit action if available.
      if (in_array('edit', $ops)) {
        $row['data']['edit'] = $role_fixed ? '' : l(t('edit'), "$this->path/manage/$name");
      }

      // Add the translate action if available.
      if (in_array('translate', $ops)) {
        $row['data']['translate'] = l(t('translate'), "$this->path/manage/$name/translate");
      }

      // Add the clone action if available.
      if (in_array('clone', $ops)) {
        $row['data']['clone'] = l(t('clone'), "$this->path/manage/$name/clone");
      }

      // Add the delete/revert action if available.
      if (in_array('delete', $ops)) {
        $overridden = entity_has_status('group_role', $group_role, ENTITY_OVERRIDDEN);

        // Don't show action for fixed or default roles.
        if ($role_fixed || $group_role->status == ENTITY_IN_CODE) {
          $row['data']['delete'] = '';
        }

        else {
          // Show revert action for overridden roles.
          if ($overridden) {
            $what = t('revert');
            $path = "$this->path/manage/$name/revert";
          }

          // Show delete action for custom roles.
          else {
            $what = t('delete');
            $path = "$this->path/manage/$name/delete";
          }

          $row['data']['delete'] = l($what, $path, array('query' => drupal_get_destination()));
        }
      }

      // Add the export action if available.
      if (in_array('export', $ops)) {
        $row['data']['export'] = l(t('export'), "$this->path/manage/$name/export");
      }
    }

    return $row;
  }
}
