<?php

/**
 * @file
 * Example page for the checkall extension to Drupal checkboxes elements.
 */

/**
 * Checkall example page.
 *
 * The form contains two separate sections. The first one contains a fieldset
 * that provides filtering options for the second one (the settings table),
 * which is only rendered when there are filtering options enabled.
 *
 * The example isn't really useful, but aimed to show how to attach the
 * checkall behaviour to different kind of checkboxes.
 *
 * Regarding the example itself, the filtering section allows the user choose
 * which months (a checkboxes element) should be rendered on the settings table.
 *
 * The settings table contains a first column with colors names, and then a new
 * column of checkboxes for each month. Each one of these cells corresponds to
 * a single option of the checkboxes element for each month, and its values are
 * just colors, so the user can choose which colors are enabled for each month.
 *
 * Note that the '#checkall' attribute is assigned to checkboxes elements with
 * a unique value that will be used to link the group of checkboxes with each
 * individual checkbox.
 *
 * For standalone checkboxes, all we needs it the '#checkall' attribute. If set
 * to TRUE, a unique CSS class is assigned internally. If set to a string, then
 * that will be used as the CSS class that links the items together.
 *
 * For chackboxes where each item is rendered on a separate table column, it is
 * required to assign a unique CSS through the '#checkall' attribute of the
 * checkboxes element. You should also assign this class to a header of the
 * table where you wish to render the check/uncheck all element.
 *
 * Tip: Search for "CHECKALL NOTE" in code comments to follow the comments
 * particulary related to the checkall feature.
 */
function checkall_example_page() {
  $form = array();

  // Initialize session storage for the options in this form.
  if (!isset($_SESSION['checkall_example'])) {
    $_SESSION['checkall_example'] = array();
  }

  $months = array(
    1 => t('January'), 2 => t('February'), 3 => t('March'),
    4 => t('April'), 5 => t('May'), 6 => t('June'),
    7 => t('July'), 8 => t('August'), 9 => t('September'),
    10 => t('October'), 11 => t('November'), 12 => t('December')
  );

  // Form filters section.
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filters'),
    '#collapsible' => !empty($_SESSION['checkall_example']['filter_by_month']),
    '#collapsed' => !empty($_SESSION['checkall_example']['filter_by_month']),
    '#description' => t('This section allows you to select which settings should be rendered on the second section of this form. The second section of the form is rendered only when one or more options have been selected here.'),
  );
  $form['filters']['filter_by_month'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Months'),
    '#options' => $months,
    '#default_value' => !empty($_SESSION['checkall_example']['filter_by_month']) ? $_SESSION['checkall_example']['filter_by_month'] : array(),
    '#description' => t('Select the months that you wish to customize.'),

    /**
     * CHECKALL NOTE:
     * --------------
     *
     * Example 1: Standalone checkboxes element.
     *
     * The following attribute enables the checkall behaviour for this
     * checkboxes element. You should use a name that's a unique CSS
     * class, so we use the module name as a prefix and a unique name
     * for this form element.
     */
    '#checkall' => 'checkall-example-filter-by-month',
  );
  $form['filters']['apply_filters'] = array(
    '#type' => 'submit',
    '#value' => t('Apply filters'),
  );

  // Done if no filters are selected.
  if (empty($_SESSION['checkall_example']['filter_by_month'])) {
    return $form;
  }

  // Setup values for the table checkboxes.
  $colors = array(
    '000000' => t('Black'), '0000ff' => t('Blue'), '00ffff' => t('Cyan'),
    'ff00ff' => t('Fuchsia'), '808080' => t('Gray'), '00ff00' => t('Lime'),
    'ff00ff' => t('Magenta'), 'ff0000' => t('Red'), 'c0c0c0' => t('Silver'),
    'fffafa' => t('Snow'), 'ffffff' => t('White'), 'ffff00' => t('Yellow'),
  );
  $form['colors'] = array(
    '#type' => 'value',
    '#value' => $colors,
  );
  $form['months'] = array(
    '#type' => 'value',
    '#value' => array_intersect_key($months, $_SESSION['checkall_example']['filter_by_month']),
  );

  // Build the table checkboxes for the selected months.
  foreach ($_SESSION['checkall_example']['filter_by_month'] as $month) {
    $form['month_colors_'. $month] = array(
      '#type' => 'checkboxes',
      '#options' => array_map(create_function('$a', 'return "";'), $colors),
      '#default_value' => !empty($_SESSION['checkall_example']['month_colors_'. $month]) ? $_SESSION['checkall_example']['month_colors_'. $month] : array(),

      /**
       * CHECKALL NOTE:
       * --------------
       *
       * Example 2 - Step 1/2: Checkboxes where each item will be rendered
       * on a separate table cell.
       *
       * The following attribute enables the checkall behaviour for this
       * checkboxes element. You should use a name that's a unique CSS
       * class, so we use the module name as a prefix and a unique name
       * for this form element.
       *
       * In addition to the standalone checkboxes example, this time we
       * need to use this same exact name in the theme function of the
       * form.
       */
      '#checkall' => 'checkall-example-month-color-'. $month,
    );
  }

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
  );
  return $form;
}

/**
 * Submit handler of the checkall example form.
 */
function checkall_example_page_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Apply filters')) {
    $_SESSION['checkall_example']['filter_by_month'] = array_filter($form_state['values']['filter_by_month']);
    drupal_set_message(t('The filters have been saved.'));
  }
  elseif ($form_state['values']['op'] == t('Save')) {
    foreach ($_SESSION['checkall_example']['filter_by_month'] as $month) {
      $_SESSION['checkall_example']['month_colors_'. $month] = array_filter($form_state['values']['month_colors_'. $month]);
    }
    drupal_set_message(t('The settings have been saved.'));
  }
  elseif ($form_state['values']['op'] == t('Reset')) {
    $_SESSION['checkall_example'] = array();
    drupal_set_message(t('The settings have been initialized.'));
  }
}

/**
 * Render the checkall example form.
 */
function theme_checkall_example_page($form) {
  drupal_add_css(drupal_get_path('module', 'checkall_example') .'/checkall_example.css', 'module', 'all', FALSE);

  // Render the filters section of the form.
  $output = drupal_render($form['filters']);

  // Done if no filters are selected.
  if (empty($_SESSION['checkall_example']['filter_by_month'])) {
    $output .= drupal_render($form);
    return $output;
  }

  // Build the settings table section of the form.
  // First column contains colors. Next, one column for each selected month.
  $output .= '<h2>'. check_plain(t('Colors per Month settings')) .'</h2>';

  // Build the table headers.
  $headers = array(
    array('data' => t('Color')),
  );
  foreach ($form['months']['#value'] as $month => $month_label) {
    // The class used here should match the name assigned to the checkboxes
    // element related to this table header.
    $headers[] = array(
      'data' => check_plain($month_label),

      /**
       * CHECKALL NOTE:
       * --------------
       *
       * Example 2 - Step 2/2: Checkboxes where each item will be rendered
       * on a separate table cell.
       *
       * We now use a CSS class name that matches the value assigned to the
       * #checkall attribute of this checkboxes element.
       *
       * Here's where the check/uncheck all checkbox will be rendered by the
       * javascript behavior, so you can use as many checkboxes as you wish
       * on the same form.
       */
      'class' => 'checkall-example-month-color-'. $month,
    );
  }

  // Build the table rows.
  $rows = array();
  foreach ($form['colors']['#value'] as $color => $color_name) {
    $cells = array(
      array('data' => check_plain($color_name)),
    );
    foreach (array_keys($form['months']['#value']) as $month) {
      if (isset($form['month_colors_'. $month][$color])) {
        $cells[] = array('data' => drupal_render($form['month_colors_'. $month][$color]));
      }
      else {
        $cells[] = array('data' => '');
      }
    }
    $rows[] = $cells;
  }

  // Render the table.
  $output .= theme('table', $headers, $rows);

  // Render the remaining elements of the form.
  $output .= drupal_render($form);

  return $output;
}
