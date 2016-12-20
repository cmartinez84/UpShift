<?php
/**
 * @file
 * Page functions for groups.
 */

/**
 * Retrieves autocomplete suggestions for existing groups.
 *
 * This function will search for all existing groups the user
 * has view access to, regardless of group type.
 *
 * @param string $search
 *   The partial title of the group.
 */
function group_autocomplete($search = '') {
  group_autocomplete_by_type(NULL, $search);
}

/**
 * Retrieves autocomplete suggestions for existing groups.
 *
 * This function will search for all existing groups the user
 * has view access to, optionally filtered by group type.
 *
 * @param GroupType $group_type
 *   (optional) The group type to retrieve suggestions for. The search will
 *   traverse all known group types when no type is provided.
 * @param string $search
 *   The partial title of the group.
 *
 * @todo Individual group access check.
 */
function group_autocomplete_by_type(GroupType $group_type = NULL, $search = '') {
  $matches = array();

  if ($search) {
    $query = db_select('groups', 'g');
    $query->addField('g', 'gid');
    $query->addField('g', 'title');
    $query->condition('title', db_like($search) . '%', 'LIKE');
    $query->range(0, 10);

    if (!empty($group_type)) {
      $query->condition('type', $group_type->name);
    }

    foreach ($query->execute()->fetchAllKeyed() as $gid => $title) {
      $title = check_plain($title);
      $match = "$title (GID: $gid)";

      $matches[$match] = $match;
    }
  }

  drupal_json_output($matches);
}

/**
 * Displays add group links for available group types.
 *
 * Redirects to group/add/[type] if only one group type is available.
 */
function group_add_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }

  if ($content) {
    $output = theme('admin_block_content', array('content' => $content));
  }
  else {
    $message = 'There are no group types available to you. Go to the <a href="@create-group">group type creation page</a> to add one or check the <a href="@group-permissions">Group module permissions</a>.';
    $replace = array(
      '@create-group' => url('admin/group/type/add'),
      '@group-permissions' => url('admin/people/permissions', array('fragment' => 'module-group')),
    );

    $output = '<p>' . t($message, $replace) . '</p>';
  }

  return $output;
}

/**
 * Generates the front page of a group.
 *
 * @param Group $group
 *   The group object.
 *
 * @return array
 *   A render array suitable for use by drupal_render().
 */
function group_page(Group $group) {
  // If there is a menu link to this group, the link becomes the last part
  // of the active trail, and the link name becomes the page title.
  // Thus, we must explicitly set the page title to be the group title.
  drupal_set_title($group->label());

  // Retrieve the URI for the group.
  $uri = entity_uri('group', $group);

  // Set the group path as the canonical URL to prevent duplicate content.
  $attributes = array(
    'rel' => 'canonical',
    'href' => url($uri['path'], $uri['options']),
  );
  drupal_add_html_head_link($attributes, TRUE);

  // Set the non-aliased path as a default shortlink.
  $attributes = array(
    'rel' => 'shortlink',
    'href' => url($uri['path'], array_merge($uri['options'], array('alias' => TRUE))),
  );
  drupal_add_html_head_link($attributes, TRUE);

  return $group->view('full', NULL, TRUE);
}
