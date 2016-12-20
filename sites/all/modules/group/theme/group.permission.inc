<?php
/**
 * @file
 * Theme functions regarding group permission pages.
 */

/**
 * Returns HTML for an individual group permission description.
 */
function theme_group_permission_description($variables) {
  if (!$variables['hide']) {
    $description = array();
    $permission = $variables['permission'];

    if (!empty($permission['description'])) {
      $description[] = $permission['description'];
    }

    if (!empty($permission['warning'])) {
      $description[] = '<em class="permission-warning">' . $permission['warning'] . '</em>';
    }

    if (!empty($description)) {
      return implode('<br />', $description);
    }
  }
}

/**
 * Returns HTML for the administer permissions page.
 */
function theme_group_permission_form($variables) {
  $form = $variables['form'];
  $group_roles = $form['roles']['#value'];

  // Shortcircuit when no roles are defined.
  if (empty($group_roles)) {
    return '<p>' . t('You have not created any roles for this group type yet. Go to the <a href="@create-role">group role creation page</a> to add a new group role.', array('@create-role' => url('admin/group/role/add'))) . '</p>';
  }

  // Theme the rows of the permission table.
  foreach (element_children($form['permissions']) as $pname) {
    $row = array();

    // Theme a module name row.
    if (is_numeric($pname)) {
      $row[] = array(
        'data' => drupal_render($form['permissions'][$pname]),
        'class' => array('module'),
        'id' => 'module-' . $form['permissions'][$pname]['#id'],
        'colspan' => count($form['roles']['#value']) + 1,
      );
    }

    // Theme a permission row.
    else {
      $row[] = array(
        'data' => drupal_render($form['permissions'][$pname]),
        'class' => array('permissions'),
      );

      foreach (element_children($form['checkboxes']) as $rname) {
        if (isset($form['checkboxes'][$rname][$pname])) {
          // Title content for both the label as the checkbox itself.
          $title = $group_roles[$rname]->label() . ': ' . $form['permissions'][$pname]['#markup'];

          $form['checkboxes'][$rname][$pname]['#title'] = $title;
          $form['checkboxes'][$rname][$pname]['#title_display'] = 'invisible';
          $form['checkboxes'][$rname][$pname]['#attributes']['title'] = $title;

          $row[] = array(
            'data' => drupal_render($form['checkboxes'][$rname][$pname]),
            'class' => array('checkbox'),
          );
        }
        else {
          $row[] = array(
            'data' => 'x',
            'class' => array('group-permission-incompatible'),
            'title' => t('"@permission" is incompatible with "@role"', array(
              '@permission' => $form['permissions'][$pname]['#markup'],
              '@role' => $group_roles[$rname]->label(),
            )),
          );
        }
      }
    }

    $rows[] = $row;
  }

  // Add css for incompatible permissions.
  drupal_add_css('.group-permission-incompatible {color: #8c2e0b;text-align:center;}', array('type' => 'inline'));

  // Compose the table header.
  $header = array(t('Permission'));
  foreach (element_children($form['roles']) as $rname) {
    $header[] = array(
      'data' => drupal_render($form['roles'][$rname]),
      'class' => array('checkbox'),
    );
  }

  // Generate and return the actual output.
  $output = theme('system_compact_link');
  $output .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'group-permissions'),
  ));
  $output .= drupal_render_children($form);

  return $output;
}
