<?php
/**
 * @file
 * Page functions for group memberships.
 */

/**
 * Generates a group membership's "profile page".
 *
 * @param GroupMembership $group_membership
 *   The group membership object.
 *
 * @return array
 *   A render array suitable for use by drupal_render().
 */
function group_membership_page(GroupMembership $group_membership) {
  // If there is a menu link to this member, the link becomes the last part
  // of the active trail, and the link name becomes the page title.
  // Thus, we must explicitly set the page title to be the member page title.
  drupal_set_title($group_membership->label(), PASS_THROUGH);

  // Retrieve the URI for the member page.
  $uri = entity_uri('group_membership', $group_membership);

  // Set the member path as the canonical URL to prevent duplicate content.
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

  return $group_membership->view('full', NULL, TRUE);
}

/**
 * Generates a group membership's metadata page.
 *
 * @param GroupMembership $group_membership
 *   The group membership object.
 *
 * @return array
 *   A render array suitable for use by drupal_render().
 */
function group_membership_metadata(GroupMembership $group_membership) {
  $rows = array();

  // Loop over entries manually to keep indexes from messing things up.
  foreach (module_invoke_all('group_membership_metadata', $group_membership) as $metadata) {
    $rows[] = array($metadata['description'], $metadata['value']);
  }

  $table = array(
    '#theme' => 'table',
    '#header' => array(
      'description' => t('Description'),
      'value' => t('Value'),
    ),
    '#rows' => $rows,
    '#empty' => t('No metadata could be retrieved for this member'),
  );

  return $table;
}
