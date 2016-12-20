<?php
/**
 * @file
 * Defines the Entity API UI class for group memberships.
 */

/**
 * UI class for group memberships.
 */
class GroupMembershipUIController extends EntityDefaultUIController {

  /**
   * Gather all group member profile info.
   *
   * @var array
   */
  protected $profile_info;

  /**
   * Count the amount of group member profiles.
   *
   * @var int
   */
  protected $profile_count;

  /**
   * Class constructor.
   */
  public function __construct($entity_type, $entity_info) {
    parent::__construct($entity_type, $entity_info);

    $this->profile_info = group_membership_profile_info();
    $this->profile_count = count($this->profile_info);
  }

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    $items[$this->path] = array(
      'title' => 'Group members',
      'description' => 'Manage group membership fields and display',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_membership_overview_form', 'group_membership'),
      'access arguments' => array('configure group module'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 5,
    );

    // Set up a dummy menu item so the Field UI tabs show up correctly.
    $items["$this->path/manage/group_membership"] = array(
      'title' => 'Manage defaults',
      'description' => 'Manage default group membership fields and display',
      'page callback' => 'drupal_goto',
      'page arguments' => array($this->path),
      'access arguments' => array('configure group module'),
    );

    // Below are router definitions for individual group memberships.
    $items['group/%group/member'] = array(
      'title' => 'Members',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_member_overview_form', 1),
      'access callback' => 'group_access',
      'access arguments' => array('access member overview', 1),
      'file' => 'admin/group_membership.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 0,
    );

    $items['group/%group_membership/member/%'] = array(
      'title callback' => 'group_membership_page_title',
      'title arguments' => array(1),
      'page callback' => 'group_membership_page',
      'page arguments' => array(1),
      'access callback' => 'group_membership_access',
      'access arguments' => array('view', 1),
      'file' => 'pages/group_membership.inc',
      'file path' => drupal_get_path('module', 'group'),
      'load arguments' => array(3),
    );

    $items['group/%group_membership/member/%/view'] = array(
      'title' => 'View',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    $items['group/%group_membership/member/%/metadata'] = array(
      'title' => 'Metadata',
      'page callback' => 'group_membership_metadata',
      'page arguments' => array(1),
      'access callback' => 'group_membership_access',
      'access arguments' => array('metadata', 1),
      'file' => 'pages/group_membership.inc',
      'file path' => drupal_get_path('module', 'group'),
      'load arguments' => array(3),
      'type' => MENU_LOCAL_TASK,
      'weight' => 5,
    );

    $items['group/%group_membership/member/%/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'entity_ui_get_form',
      'page arguments' => array('group_membership', 1),
      'access callback' => 'group_membership_access',
      'access arguments' => array('update', 1),
      'file' => 'forms/group_membership.inc',
      'file path' => drupal_get_path('module', 'group'),
      'load arguments' => array(3),
      'type' => MENU_LOCAL_TASK,
      'weight' => 10,
    );

    $items['group/%group_membership/member/%/cancel'] = array(
      'title' => 'Cancel membership',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('group_membership_cancel_form', 1),
      'access callback' => 'group_membership_access',
      'access arguments' => array('delete', 1),
      'file' => 'admin/group_membership.inc',
      'file path' => drupal_get_path('module', 'group'),
      'load arguments' => array(3),
      'type' => MENU_CALLBACK,
    );

    // Below are router definitions for joining a group.
    $items['group/%group/member/%user/join'] = array(
      'title' => 'Join group',
      'title callback' => 'group_membership_join_title',
      'title arguments' => array(1),
      'page callback' => 'group_membership_join_form_builder',
      'page arguments' => array(1, 3, 'group_membership_join_form'),
      'access callback' => 'group_membership_join_form_access',
      'access arguments' => array(1, 3),
      'file' => 'forms/group_membership.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    $items['group/%group/member/%user/request'] = array(
      'title' => 'Request group membership',
      'title callback' => 'group_membership_request_title',
      'title arguments' => array(1),
      'page callback' => 'group_membership_join_form_builder',
      'page arguments' => array(1, 3, 'group_membership_request_form'),
      'access callback' => 'group_membership_request_form_access',
      'access arguments' => array(1, 3),
      'file' => 'forms/group_membership.inc',
      'file path' => drupal_get_path('module', 'group'),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }

  /**
   * Provides definitions for implementing hook_forms().
   *
   * Overridden to limit the output.
   */
  public function hook_forms() {
    $forms['group_membership_overview_form'] = array(
      'callback' => 'entity_ui_overview_form',
      'wrapper_callback' => 'entity_ui_form_defaults',
    );

    $forms['group_membership_join_form'] = array(
      'callback' => 'group_membership_form',
    );

    $forms['group_membership_request_form'] = array(
      'callback' => 'group_membership_form',
    );

    return $forms;
  }

  /**
   * Builds the entity overview form.
   */
  public function overviewForm($form, &$form_state) {
    $form['table'] = $this->profileTable();
    return $form;
  }

  /**
   * Generates the render array for an overview table for member profiles.
   *
   * @return array
   *   A renderable array.
   */
  protected function profileTable() {
    $rows = array();

    foreach ($this->profile_info as $name => $profile) {
      $row = $this->profileTableRow($name, $profile);
      $rows[$name] = $row;
    }

    $table = array(
      '#theme' => 'table',
      '#header' => $this->profileTableHeaders(),
      '#rows' => $rows,
      '#attributes' => array('id' => 'group-member-profile-table'),
    );

    return $table;
  }

  /**
   * Returns the available operations for the page.
   */
  protected function profileOperations() {
    $field_ui = module_exists('field_ui');

    // If only the default profile exists, return just Field UI operations.
    if ($this->profile_count == 1) {
      return $field_ui ? array('manage fields', 'manage display') : array();
    }

    $ops = array('edit');
    if ($field_ui) {
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
  protected function profileTableHeaders() {
    $op_count = count($this->profileOperations());

    $header = array(
      'label' => t('Name'),
      'status' => t('Status'),
    );

    // Add the actions column only when necessary.
    if ($op_count > 0) {
      $header['actions'] = array(
        'data' => t('Actions'),
        'colspan' => $op_count,
      );
    }

    return $header;
  }

  /**
   * Generates the row for the passed type.
   */
  protected function profileTableRow($name, $profile) {
    $profile_fixed = ($profile['entity status'] & ENTITY_FIXED) == ENTITY_FIXED;
    $profile_default = $name == 'group_membership';

    // Add the type label.
    $row['label'] = $profile['label'];

    // Add the status column.
    $row['status'] = array(
      'data' => array(
        '#theme' => 'entity_status',
        '#status' => $profile['entity status'],
      ),
    );

    if ($this->profile_count > 1) {
      // Add the edit action.
      $row['edit'] = $profile_fixed ? '' : l(t('edit'), $profile['path']);
    }

    // Add the Field UI actions.
    if (module_exists('field_ui')) {
      $row['manage fields'] = l(t('manage fields'), $profile['path'] .'/fields');
      $row['manage display'] = l(t('manage display'), $profile['path'] . '/display');
    }

    if ($this->profile_count > 1) {
      // Add the translation action.
      if (module_exists('i18n_string')) {
        $row['translate'] = $profile_default ? '' : l(t('translate'), $profile['path'] . '/translate');
      }

      // Add the clone action.
      $row['clone'] = $profile_default ? '' : l(t('clone'), $profile['path'] . '/clone');

      // Don't show delete action for fixed or default types.
      if ($profile_fixed || $profile['entity status'] == ENTITY_IN_CODE) {
        $row['delete'] = '';
      }

      // Add the delete/revert action.
      else {
        // Show revert action for overridden roles.
        if (entity_has_status('group_type', $profile, ENTITY_OVERRIDDEN)) {
          $what = t('revert');
          $path = $profile['path'] . '/revert';
        }

        // Show delete action for custom types.
        else {
          $what = t('delete');
          $path = $profile['path'] . '/delete';
        }

        $row['delete'] = $profile_default ? '' : l($what, $path, array('query' => drupal_get_destination()));
      }

      // Add the export action.
      $row['export'] = $profile_default ? '' : l(t('export'), $profile['path'] . '/export');
    }

    return $row;
  }
}
