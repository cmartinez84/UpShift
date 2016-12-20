<?php
/**
 * @file
 * Hook implementations for the Pathauto module.
 */

/**
 * Implements hook_path_alias_types().
 *
 * Here we return what the original path for a group is so Pathauto can delete
 * them in one go.
 */
function group_path_alias_types() {
  $alias_types['group/'] = t('Group');
  return $alias_types;
}

/**
 * Implements hook_pathauto().
 */
function group_pathauto($op) {
  if ($op != 'settings') {
    return;
  }

  if (!module_exists('entity_token')) {
    return;
  }

  $settings = array(
    'module' => 'group',
    'token_type' => 'group',
    'groupheader' => t('Group paths'),
    'patterndescr' => t('Default path pattern (applies to all group types with blank patterns below)'),
    'patterndefault' => 'group/[group:title]',
    'batch_update_callback' => 'group_pathauto_bulk_update_batch_process',
    'batch_file' => drupal_get_path('module', 'group') . '/group.pathauto.inc',
  );

  $languages = array();
  if (module_exists('locale')) {
    $languages = array(LANGUAGE_NONE => t('language neutral')) + locale_language_list('name');
  }

  foreach (group_type_labels() as $name => $label) {
    if (count($languages)) {
      $settings['patternitems'][$name] = t('Default path pattern for @group_type (applies to all @group_type groups with blank patterns below)', array('@group_type' => $label));

      foreach ($languages as $lang_code => $lang_name) {
        $settings['patternitems'][$name . '_' . $lang_code] = t('Pattern for all @language @group_type paths', array('@group_type' => $label, '@language' => $lang_name));
      }
    }
    else {
      $settings['patternitems'][$name] = t('Pattern for all @group_type paths', array('@group_type' => $label));
    }
  }

  return (object) $settings;
}

/**
 * Batch processing callback; Generate aliases for groups.
 */
function group_pathauto_bulk_update_batch_process(&$context) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['count'] = 0;
    $context['sandbox']['current'] = 0;
  }

  $query = db_select('groups', 'g');
  $query->leftJoin('url_alias', 'ua', "CONCAT('group/', g.gid) = ua.source");
  $query->addField('g', 'gid');
  $query->isNull('ua.source');
  $query->condition('g.gid', $context['sandbox']['current'], '>');
  $query->orderBy('g.gid');
  $query->addTag('pathauto_bulk_update');
  $query->addMetaData('entity', 'group');

  // Get the total amount of items to process.
  if (!isset($context['sandbox']['total'])) {
    $context['sandbox']['total'] = $query->countQuery()->execute()->fetchField();

    // If there are no groups to update, the stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  $query->range(0, 25);
  $gids = $query->execute()->fetchCol();

  group_pathauto_update_alias_multiple($gids, 'bulkupdate');
  $context['sandbox']['count'] += count($gids);
  $context['sandbox']['current'] = max($gids);
  $context['message'] = t('Updated alias for group @gid.', array('@gid' => end($gids)));

  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}

/**
 * Update the URL aliases for multiple groups.
 *
 * @param array $gids
 *   An array of group IDs.
 * @param string $op
 *   Operation being performed on the nodes ('insert', 'update' or
 *   'bulkupdate').
 * @param array $options
 *   (optional) An array of additional options.
 */
function group_pathauto_update_alias_multiple(array $gids, $op, array $options = array()) {
  $options += array('message' => FALSE);

  $groups = group_load_multiple($gids);
  foreach ($groups as $group) {
    group_pathauto_update_alias($group, $op);
  }

  if (!empty($options['message'])) {
    drupal_set_message(format_plural(count($gids), 'Updated URL alias for 1 group.', 'Updated URL aliases for @count groups.'));
  }
}

/**
 * Creates or updates entity alias.
 *
 * @param Group $group
 *   The group the alias is being created/updated for.
 * @param string $op
 *   Operation being performed on the alias ('insert', 'update' or 'bulkupdate').
 * @param array $language
 *   Additional parameter to set the language of the path.
 *
 * @return string
 *   The alias that was created.
 */
function group_pathauto_update_alias(Group $group, $op, $language = NULL) {
  // Skip processing if the user has disabled pathauto for the group.
  if (isset($group->path['pathauto']) && empty($group->path['pathauto'])) {
    return;
  }

  // Make sure the language is set.
  if (!isset($language)) {
    $language = entity_language('group', $group);

    // To preserve some backwards compatibility, entity_language() returns NULL
    // instead of LANGUAGE_NONE when no language is found. Pathauto expects
    // LANGUAGE_NONE, though, so we convert it.
    if (!isset($language)) {
      $language = LANGUAGE_NONE;
    }
  }

  // Skip processing if the entity has no pattern.
  if (!pathauto_pattern_load_by_entity('group', $group->type, $language)) {
    return;
  }

  module_load_include('inc', 'pathauto');
  $uri = entity_uri('group', $group);
  return pathauto_create_alias('group', $op, $uri['path'], array('group' => $group), $group->type, $language);
}
