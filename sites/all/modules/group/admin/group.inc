<?php
/**
 * @file
 * Group overview admin UI.
 *
 * @see GroupUIController::overviewForm()
 * @see GroupUIController::overviewFormSubmit()
 */

/**
 * Builds the group overview filters.
 */
function group_filters_form() {
  $filters = module_invoke_all('group_filters');

  $session = isset($_SESSION['group_overview_filters'])
    ? $_SESSION['group_overview_filters']
    : array();

  $i = 0;
  $fs_filters = array(
    '#type' => 'fieldset',
    '#title' => t('Show only groups where'),
    '#theme' => 'exposed_filters__group',
  );

  // Loop through session saved filters.
  foreach ($session as $type => $value) {
    $value = $filters[$type]['options'][$value];

    $t_args = array(
      '%property' => $filters[$type]['title'],
      '%value' => $value,
    );

    $fs_filters['current'][] = ($i++)
      ? array('#markup' => t('and where %property is %value', $t_args))
      : array('#markup' => t('where %property is %value', $t_args));

    // Remove the option if it is already being filtered on.
    unset($filters[$type]);
  }

  if (count($filters)) {
    $fs_filters['status'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('clearfix')),
      '#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
    );
  }

  $fs_filters['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
  );

  foreach ($filters as $key => $filter) {
    $fs_filters['status']['filters'][$key] = array(
      '#type' => 'select',
      '#options' => $filter['options'],
      '#title' => $filter['title'],
      '#default_value' => '[any]',
    );
  }

  $fs_filters['status']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );

  if (count($filters)) {
    $fs_filters['status']['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => count($session) ? t('Refine') : t('Filter'),
    );
  }

  if (count($session)) {
    $fs_filters['status']['actions']['undo'] = array(
      '#type' => 'submit',
      '#value' => t('Undo'),
    );

    $fs_filters['status']['actions']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
    );
  }

  return $fs_filters;
}

/**
 * Filter form submit callback.
 */
function group_filters_form_submit($form, &$form_state) {
  $filters = module_invoke_all('group_filters');

  switch ($form_state['values']['op']) {
    case t('Filter'):
    case t('Refine'):
      // Apply every filter that has a choice selected other than 'any'.
      foreach ($filters as $filter => $options) {
        if (isset($form_state['values'][$filter]) && $form_state['values'][$filter] != '[any]') {
          // Flatten the options array to accommodate hierarchical/nested options.
          $flat_options = form_options_flatten($filters[$filter]['options']);

          // Only accept valid selections offered on the dropdown, block bad input.
          if (isset($flat_options[$form_state['values'][$filter]])) {
            $_SESSION['group_overview_filters'][$filter] = $form_state['values'][$filter];
          }
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['group_overview_filters']);
      break;
    case t('Reset'):
      $_SESSION['group_overview_filters'] = array();
      break;
  }
}

/**
 * Builds the group administration options.
 */
function group_options_form() {
  $fs_options = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );

  if (!user_access('bypass group access')) {
    $fs_options['#access'] = FALSE;
  }
  else {
    $options = array();
    foreach (module_invoke_all('group_operations') as $operation => $array) {
      if (!empty($array['optgroup'])) {
        $options[$array['optgroup']][$operation] = $array['label'];
      }
      else {
        $options[$operation] = $array['label'];
      }
    }

    $fs_options['operation'] = array(
      '#type' => 'select',
      '#title' => t('Operation'),
      '#title_display' => 'invisible',
      '#options' => $options,
      '#default_value' => 'approve',
    );

    $fs_options['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#validate' => array('group_options_form_validate'),
      '#submit' => array('group_options_form_submit'),
    );
  }

  return $fs_options;
}

/**
 * Options form validate callback.
 */
function group_options_form_validate($form, &$form_state) {
  // Error if there are no groups selected.
  if (!is_array($form_state['values']['groups']) || !count(array_filter($form_state['values']['groups']))) {
    form_set_error('', t('No groups selected.'));
  }
}

/**
 * Options form submit callback.
 */
function group_options_form_submit($form, &$form_state) {
  $operations = module_invoke_all('group_operations');
  $operation = $operations[$form_state['values']['operation']];

  // Get all selected groups.
  $gids = array_filter($form_state['values']['groups']);

  // Build the arguments for the operation callback.
  $args = array($gids);
  if (isset($operation['callback arguments'])) {
    $args = array_merge($args, $operation['callback arguments']);
  }

  if (empty($operation['form callback'])) {
    call_user_func_array($operation['callback'], $args);
  }
  else {
    // Pass the operation form details.
    $form_state['rebuild_info']['callback'] = $operation['callback'];
    $form_state['rebuild_info']['args'] = $args;

    // We need to rebuild the form to be able to show the operation form.
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Builds the group overview table.
 *
 * @param int $pager_limit
 *   The pager limit for the result page.
 *
 * @todo Individual group access check on query.
 */
function group_groups_form($pager_limit) {
  $options = array();
  $header = array(
    'title' => array(
      'data' => t('Group name'),
      'field' => 'title',
      'sort' => 'asc',
    ),
    'type' => array(
      'data' => t('Group type'),
      'field' => 'type',
    ),
    'members' => array(
      'data' => t('Members'),
      'field' => 'members',
    ),
    'operations' => t('Operations'),
  );

  // Below we build a query that is used to retrieve group ids based on
  // filters, a pager and group access. Additionally, we add a 'fake' field
  // that lists the amount of members so we can sort on that too.
  $query = db_select('groups', 'g')->extend('PagerDefault')->extend('TableSort');
  $query->leftJoin('group_membership', 'gm', 'g.gid=gm.gid');
  $query->addField('g', 'gid');
  $query->addExpression('COUNT(distinct gm.uid)', 'members');
  $query->groupBy('g.gid');
  $query->orderByHeader($header);
  $query->limit($pager_limit);

  // Retrieve all active filters.
  $filters = isset($_SESSION['group_overview_filters']) ? $_SESSION['group_overview_filters'] : array();

  // Allow other modules to alter the query by passing on the filters.
  $query->addTag('group_overview');
  $query->addMetaData('filters', $filters);

  // Add the groups to the table.
  foreach (group_load_multiple($query->execute()->fetchCol()) as $group) {
    $options[$group->gid] = array(
      'title' => l($group->label(), "group/$group->gid"),
      'type' => group_type_load($group->type)->label(),
      'members' => $group->getMemberCount(),
    );

    // Start with an empty operations array.
    $options[$group->gid]['operations'] = array();

    // Gather all operation links.
    $operations = module_invoke_all('group_operation_links', $group);

    if (count($operations) > 1) {
      // Render an unordered list of operations links.
      $options[$group->gid]['operations'] = array(
        'data' => array(
          '#theme' => 'links__group_operation_links',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    }
    elseif (!empty($operations)) {
      // Render the first and only operation as a link.
      $link = reset($operations);

      // Pass in $link as $options, they share the same keys.
      $options[$group->gid]['operations'] = l($link['title'], $link['href'], $link);
    }
  }

  $form['groups'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No groups available'),
  );

  // Transform into a table if the user can't perform any operations.
  if (!user_access('bypass group access')) {
    unset($form['groups']['#type'], $form['groups']['#options']);
    $form['groups']['#theme'] = 'table';
    $form['groups']['#rows'] = $options;
  }

  return $form;
}

/**
 * Multiple group deletion confirmation form.
 */
function group_multiple_delete_confirm($form, &$form_state, $gids) {
  $form['#submit'][] = 'group_multiple_delete_confirm_submit';

  $form['groups'] = array(
    '#prefix' => '<ul>',
    '#suffix' => '</ul>',
    '#tree' => TRUE,
  );

  foreach ($gids as $gid) {
    $title = db_query('SELECT title FROM {groups} WHERE gid = :gid', array(':gid' => $gid))->fetchField();
    $form['groups'][$gid] = array(
      '#type' => 'hidden',
      '#value' => $gid,
      '#prefix' => '<li>',
      '#suffix' => check_plain($title) . "</li>\n",
    );
  }

  $form['info'] = array(
    '#markup' => t('By deleting a group you will delete all of its subgroups and content as well.'),
    '#suffix' => '<br />',
  );

  $question = format_plural(count($gids),
    'Are you sure you want to delete this group?',
    'Are you sure you want to delete these groups?'
  );

  return confirm_form($form, $question, 'admin/group', NULL, t('Delete'));
}

/**
 * Multiple group deletion submit callback.
 */
function group_multiple_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $gids = array_keys($form_state['values']['groups']);
    $count = count($gids);

    group_delete_multiple($gids);
    watchdog('groups', 'Deleted @count groups.', array('@count' => $count));
    drupal_set_message(format_plural($count, 'Deleted 1 group.', 'Deleted @count groups.'));
  }
}
