<?php
/**
 * @file
 * Page callbacks for groupmenu.
 */

/**
 * Menu callback to list the menus for the group.
 */
function groupmenu_overview_page($gid) {
  // Set the title of the page.
  $group = group_load($gid);
  $entity_label = $group->label() ? $group->label() : $gid;
  drupal_set_title(t('List menus for %title', array('%title' => $entity_label)), PASS_THROUGH);

  $header = array(
    t('Title'),
    array(
      'data' => t('Operations'),
      'colspan' => '3',
    ),
  );
  $rows = array();
  foreach (groupmenu_get_menus($gid) as $menu) {
    $row = array(
      theme('menu_admin_overview', array(
        'title' => $menu->title,
        'name' => $menu->menu_name,
        'description' => $menu->description,
      )),
    );
    $row[] = array('data' => l(t('list links'), 'group/' . $gid . '/menus/' . $menu->menu_name));
    $row[] = array('data' => l(t('edit menu'), 'group/' . $gid . '/menus/' . $menu->menu_name . '/edit'));
    $row[] = array('data' => l(t('add link'), 'group/' . $gid . '/menus/' . $menu->menu_name . '/add'));
    $rows[] = $row;
  }

  if (empty($rows)) {
    return t('There are currently no menus.');
  }
  else {
    return theme('table', array('header' => $header, 'rows' => $rows));
  }
}

/**
 * Menu callback.
 *
 * Build the form that handles the adding/editing of a custom menu.
 */
function groupmenu_edit_menu_form($form, &$form_state, $type, $gid, $menu = array()) {
  module_load_include('inc', 'menu', 'menu.admin');
  $group = group_load($gid);
  $label = $group->label();

  // Set the title of the page.
  switch ($type) {
    case 'add': drupal_set_title(t('Add menu for @title', array('@title' => $label)), PASS_THROUGH);
      break;

    case 'edit': drupal_set_title(t('Edit menu for @title', array('@title' => $label)), PASS_THROUGH);
      break;
  }

  // Build the form.
  $form = array();
  $form = menu_edit_menu($form, $form_state, $type, $menu);
  $form['groupmenu_gid'] = array(
    '#type' => 'value',
    '#value' => $gid,
  );
  if ($type == 'edit') {
    $form['groupmenu_name'] = array(
      '#type' => 'value',
      '#value' => $menu['menu_name'],
    );
  }
  // Add submit handlers.
  $form['#submit'][] = 'menu_edit_menu_submit';
  $form['#submit'][] = 'groupmenu_edit_menu_form_submit';
  $form['#submit'][] = 'groupmenu_edit_menu_form_submit_redirect';
  $form['actions']['delete']['#submit'][0] = 'groupmenu_delete_menu_form_submit';
  return $form;
}

/**
 * Submit handler for groupmenu_edit_menu_form().
 */
function groupmenu_delete_menu_form_submit($form, &$form_state) {
  $gid = $form['groupmenu_gid']['#value'];
  $menu_name = $form['groupmenu_name']['#value'];
  $form_state['redirect'] = 'group/' . $gid . '/menus/' . $menu_name . '/delete';
}

/**
 * Submit handler which handles redirection for groupmenu_edit_menu_form form.
 */
function groupmenu_edit_menu_form_submit_redirect($form, &$form_state) {
  $gid = $form['groupmenu_gid']['#value'];
  $menu_name = $form['#insert'] ? $menu_name = 'menu-' . $form['menu_name']['#value'] : $form['groupmenu_name']['#value'];
  $form_state['redirect'] = 'group/' . $gid . '/menus/' . $menu_name;
}

/**
 * Form callback which shows an entire menu tree at once.
 */
function groupmenu_overview_form($form, &$form_state, $gid, $menu) {
  module_load_include('inc', 'menu', 'menu.admin');
  drupal_set_title(t('List items for menu !mtitle', array('!mtitle' => $menu['title'])), PASS_THROUGH);
  $group = group_load($gid);
  $label = $group->label();
  // Set the breadcrumb.
  $breadcrumbs = array();
  $breadcrumbs[] = l(t('Home'), '/');
  $breadcrumbs[] = l($label, 'group/' . $gid);
  $breadcrumbs[] = l(t('Menus'), 'group/' . $gid . '/menus');
  drupal_set_breadcrumb($breadcrumbs);

  $form = menu_overview_form($form, $form_state, $menu);
  $form['#theme'] = 'menu_overview_form';
  $form['#empty_text'] = t('There are no menu links yet. !add_link', array('!add_link' => l(t('Add link'), 'group/' . $gid . '/menus/' . $menu['menu_name'] . '/add')));

  foreach (element_children($form) as $mlid) {
    if (strstr($mlid, 'mlid:')) {
      $item = $form[$mlid]['#item'];
      $operations = array();
      $operations['edit'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => 'group/' . $gid . '/menus/' . $menu['menu_name'] . '/item/' . $item['mlid'] . '/edit',
      );

      if ($item['module'] == 'menu' || $item['updated'] == 1) {
        $operations['delete'] = array(
          '#type' => 'link',
          '#title' => t('delete'),
          '#href' => 'group/' . $gid . '/menus/' . $menu['menu_name'] . '/item/' . $item['mlid'] . '/delete',
        );
      }
      $form[$mlid]['operations'] = $operations;
    }
  }
  return $form;
}

/**
 * Submit function; does the same as the normal menu_overview_form_submit.
 */
function groupmenu_overview_form_submit($form, &$form_state) {
  menu_overview_form_submit($form, $form_state);
}

/**
 * Form callback for node/%node/menus/%menu/delete.
 */
function groupmenu_delete_menu_confirm($form, &$form_state, $menu, $gid) {
  module_load_include('inc', 'menu', 'menu.admin');
  $form = menu_delete_menu_confirm($form, $form_state, $menu);
  $form['groupmenu_gid'] = array(
    '#type' => 'value',
    '#value' => $gid,
  );
  $form['actions']['cancel']['#href'] = 'group/' . $gid . '/menus/' . $menu['menu_name'];
  $form['#submit'][] = 'menu_delete_menu_confirm_submit';
  $form['#submit'][] = 'groupmenu_delete_menu_confirm_submit';
  $form['#submit'][] = 'groupmenu_delete_menu_confirm_submit_redirect';
  return $form;
}

/**
 * Form callback; Build the menu link editing form.
 */
function groupmenu_edit_item_form($form, &$form_state, $type, $gid, $menu, $item = array()) {
  module_load_include('inc', 'menu', 'menu.admin');
  $form = menu_edit_item($form, $form_state, $type, $item, $menu);
  $group = group_load($gid);
  $label = $group->label();
  // Set the breadcrumb now to override menu_edit_item() breadcrumb.
  $breadcrumbs = array();
  $breadcrumbs[] = l(t('Home'), '/');
  $breadcrumbs[] = l($label, 'group/' . $gid);
  $breadcrumbs[] = l(t('Menus'), 'group/' . $gid . '/menus');
  $breadcrumbs[] = l($menu['title'], 'group/' . $gid . '/menus/' . $menu['menu_name']);
  drupal_set_breadcrumb($breadcrumbs);

  $list = array();
  $menus = groupmenu_get_group_menus(array($gid));
  foreach ($menus as $option) {
    $list[$option['menu_name']] = $option['title'];
  }

  // Set the title of the page.
  drupal_set_title(t('Add item into menu !mtitle', array('!mtitle' => $menu['title'])), PASS_THROUGH);

  // Build the form.
  $form['parent']['#options'] = menu_parent_options($list, array('mlid' => 0));
  $form['groupmenu_gid'] = array(
    '#type' => 'value',
    '#value' => $gid,
  );
  $form['groupmenu_name'] = array(
    '#type' => 'value',
    '#value' => $menu['menu_name'],
  );
  $form['#submit'][] = 'menu_edit_item_submit';
  $form['#submit'][] = 'groupmenu_redirect';
  $form['#validate'][] = 'menu_edit_item_validate';
  if (!empty($item)) {
    $form['actions']['delete'] = array(
      '#type' => 'link',
      '#title' => 'Delete',
      '#href' => 'group/' . $gid . '/menus/' . $menu['menu_name'] . '/item/' . $item['mlid'] . '/delete',
      '#weight' => 10,
    );
  }

  return $form;
}

/**
 * Form callback for group/%/menus/%menu/item/%menu_link/delete page.
 */
function groupmenu_delete_item_form($form, &$form_state, $item, $gid, $menu) {
  module_load_include('inc', 'menu', 'menu.admin');
  $form = menu_item_delete_form($form, $form_state, $item);
  $form_state['values']['link_path'] = $form['#item']['link_path'];
  $form['groupmenu_gid'] = array(
    '#type' => 'value',
    '#value' => $gid,
  );
  $form['groupmenu_name'] = array(
    '#type' => 'value',
    '#value' => $menu['menu_name'],
  );
  $form['#submit'][] = 'menu_item_delete_form_submit';
  $form['#submit'][] = 'groupmenu_redirect';
  $form['actions']['cancel']['#href'] = 'group/' . $gid . '/menus/' . $menu['menu_name'];
  return $form;
}

/**
 * Callback for admin/structure/groupmenu.
 *
 * Just duplicates the standard menu list, but only displays those created
 * through group_menu.
 */
function groupmenu_admin_menu_overview_form($form, $form_state) {
  $session = isset($_SESSION['groupmenu_filter']) ? $_SESSION['groupmenu_filter'] : array();

  // Get filter value from $_SESSION variable.
  foreach ($session as $filter) {
    list($type, $value) = $filter;
  }

  $form['filters'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => (isset($type)) ? FALSE : TRUE,
    '#title' => 'Filter',
    '#description' => t('Filter on the title of the Group menu.'),
  );

  $form['filters']['groupmenu_filter'] = array(
    '#type' => 'textfield',
    '#title' => t('Filter this out'),
    '#required' => FALSE,
    '#size' => 20,
    '#default_value' => isset($type) ? $value : '',
  );

  $form['filters']['groupmenu_actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['filters']['groupmenu_actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  if (count($session)) {
    $form['filters']['groupmenu_actions']['reset'] = array('#type' => 'submit', '#value' => t('Reset'));
  }

  $header = array(
    'title' => array('data' => 'Title', 'field' => 'title', 'sort' => 'asc'),
    'name' => array('data' => 'Group Name', 'field' => 'group_name'),
    'list' => array('data' => 'Operations', 'colspan' => '4'),
  );

  // Build Query (extend for paging and sorting).
  $query = db_select('menu_custom', 'mc')->extend('PagerDefault')->limit(20);
  // Pager Extender.
  $query->join('group_menu', 'gm', 'mc.menu_name = gm.menu_name');
  $query->join('groups', 'g', 'g.gid = gm.gid');
  $query->fields('mc')
    ->fields('gm')
    ->orderBy('g.title')
    ->extend('TableSort')
    ->orderByHeader($header);
  $query->addField('g', 'title', 'group_title');

  // Add conditional filter if enter by user.
  if (isset($type)) {
    $query->condition('mg.title', '%' . $value . '%', 'LIKE');
  }

  $result = $query->execute();

  // Build table.
  $options = array();
  foreach ($result as $menu) {
    $options[$menu->menu_name] = array(
      'title' => array(
        'data' => theme('menu_admin_overview', array(
          'title' => $menu->title,
          'name' => $menu->menu_name,
          'description' => $menu->description,
        )),
      ),
      'name' => array('data' => $menu->group_title),
      'list' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('list links'),
          '#href' => 'admin/structure/menu/manage/' . $menu->menu_name,
        ),
      ),
      'edit' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('edit menu'),
          '#href' => 'admin/structure/menu/manage/' . $menu->menu_name . '/edit',
        ),
      ),
      'add' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('add link'),
          '#href' => 'admin/structure/menu/manage/' . $menu->menu_name . '/add',
        ),
      ),
      'view' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('view group'),
          '#href' => 'group/' . $menu->gid,
        ),
      ),
    );
  }

  $tableselect = array_sum(module_invoke_all('groupmenu_admin_menu_overview_form_tableselect'));

  if ($tableselect) {
    $form['groupmenu_menu_list'] = array(
      '#type' => 'tableselect',
      '#title' => t('Results'),
      '#header' => $header,
      '#options' => $options,
      '#empty' => 'Nothing to see',
      '#suffix' => theme('pager'),
    );
  }
  else {
    $output = theme('table', array('header' => $header, 'rows' => $options)) . theme('pager');
    $form['groupmenu_menu_list'] = array(
      '#type' => 'markup',
      '#title' => t('Results'),
      '#markup' => $output,
    );
  }

  return $form;
}

/**
 * Process result from Group Menu administration filter form.
 */
function groupmenu_admin_menu_overview_form_submit($form, &$form_state) {
  switch ($form_state['clicked_button']['#id']) {
    case 'edit-submit':
      if (isset($form_state['values']['groupmenu_filter']) && $form_state['values']['groupmenu_filter'] != '') {
        $_SESSION['groupmenu_filter'][] = array('groupmenu_filter', $form_state['values']['groupmenu_filter']);
      }
      break;

    case 'edit-reset':
      $_SESSION['groupmenu_filter'] = array();
      break;
  }
}
