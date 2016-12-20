<?php
/**
 * @file
 * Theme functions regarding subgroup config pages.
 *
 * @todo Add in JavaScript that auto-selects "member" when you select any
 *       other role to inherit?
 */

/**
 * Returns HTML for a subgroup inheritance table.
 */
function theme_ggroup_inheritance_table($variables) {
  $element = $variables['element'];

  $sources = $element['sources']['#value'];
  $targets = $element['targets']['#value'];

  // Theme the rows of the inheritance table.
  foreach ($sources as $source => $label) {
    $row = array();

    $message = 'People who were %role will receive';
    $replace = array('%role' => $label);

    $row[] = array(
      'data' => t($message, $replace),
      'class' => array('source-roles'),
    );

    foreach (array_keys($targets) as $target) {
      // Title content for both the label as the checkbox itself.
      $message = '@source will receive @target';
      $replace = array('@source' => $label, '@target' => $targets[$target]);
      $title = t($message, $replace);

      $element[$source][$target]['#title'] = $title;
      $element[$source][$target]['#title_display'] = 'invisible';
      $element[$source][$target]['#attributes']['title'] = $title;

      $row[] = array(
        'data' => drupal_render($element[$source][$target]),
        'class' => array('checkbox'),
      );
    }

    $rows[] = $row;
  }

  // Compose the table header.
  $header = array(t('Configure inheritance'));

  foreach ($targets as $target) {
    $header[] = array(
      'data' => $target,
      'class' => array('checkbox'),
    );
  }

  // Generate and return the actual output.
  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
  ));
  $output .= drupal_render_children($element);

  return $output;
}
