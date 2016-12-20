<?php
/**
 * @file
 * Definition of group_handler_field_membership_role.
 */

/**
 * Field handler to provide a list of group roles.
 *
 * Normally this handler would be placed on the {group_membership_role} table.
 * However, because we are trying to get just one combined result per group
 * membership, we need to avoid a many-to-one result. We therefore use this
 * handler on the {group_membership} table instead and run a subquery inside
 * the handler to achieve our goal.
 *
 * @ingroup views_field_handlers
 */
class group_handler_field_membership_role extends views_handler_field_prerender_list {

  function pre_render(&$values) {
    $mids = array();
    $this->items = array();

    foreach ($values as $result) {
      $mids[] = $this->get_value($result, NULL, TRUE);
    }

    if ($mids) {
      $labels = group_role_labels();

      $roles = db_select('group_membership_role', 'gmr')
        ->fields('gmr', array('mid', 'role'))
        ->condition('mid', $mids, 'IN')
        ->execute();

      foreach ($roles as $role) {
        $this->items[$role->mid][$role->role]['label'] = $labels[$role->role];
        $this->items[$role->mid][$role->role]['name'] = $role->role;
      }
    }
  }

  function render_item($count, $item) {
    return $item['label'];
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-label]'] = t('The human readable name of the group role.');
    $tokens['[' . $this->options['id'] . '-name]'] = t('The machine name of the group role.');
  }

  function add_self_tokens(&$tokens, $item) {
    if (!empty($item['label'])) {
      $tokens['[' . $this->options['id'] . '-label]'] = $item['label'];
      $tokens['[' . $this->options['id'] . '-name]'] = $item['name'];
    }
  }
}
