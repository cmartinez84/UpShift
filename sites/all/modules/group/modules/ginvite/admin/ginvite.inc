<?php
/**
 * @file
 * Group member invite admin callback and pages.
 */

/**
 * Generate the e-mail invitation list.
 *
 * @param Group $group
 *   The group to generate the list for.
 *
 * @return array
 *   The render array for this page.
 */
function ginvite_by_mail_list(Group $group) {
  $invites = ginvite_get_group_invites($group);

  $page['info'] = array(
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  if (empty($invites)) {
    $page['info']['#markup'] = t("There are no e-mail invitations that haven't been accepted yet.");
  }
  else {
    $page['info']['#markup'] = t("Below you will find a list of unclaimed e-mail invitations.<br />You may still edit or delete them as long as they remain unclaimed.");

    $rows = array();
    $group_roles = group_role_labels();

    foreach ($invites as $iid => $data) {
      // Build the invitation's roles list.
      $roles = array_intersect_key($group_roles, array_flip(unserialize($data->roles)));

      // Build the operation links.
      $destination = drupal_get_destination();

      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => "group/$group->gid/invite/mail/$iid/edit",
        'query' => $destination,
      );

      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => "group/$group->gid/invite/mail/$iid/delete",
        'query' => $destination,
      );

      // Populate a table row with all this data.
      $rows[] = array(
        $data->mail,
        array(
          'data' => array(
            '#theme' => 'item_list__group_roles',
            '#items' => $roles,
          ),
        ),
        l(format_username(user_load($data->invited_by)), "user/$data->invited_by"),
        format_date($data->invited_on),
        array(
          'data' => array(
            '#theme' => 'links__ginvite_mail_operation_links',
            '#links' => $operations,
            '#attributes' => array('class' => array('links', 'inline')),
          ),
        ),
      );
    }

    $page['invites'] = array(
      '#theme' => 'table',
      '#header' => array(
        t('E-mail'),
        t('Roles'),
        t('Invited by'),
        t('Invited on'),
        t('Operations'),
      ),
      '#rows' => $rows,
    );
  }

  return $page;
}

/**
 * Group e-mail invite edit form.
 */
function ginvite_by_mail_edit_form($form, &$form_state, Group $group, $iid) {
  $data = db_select('group_invite', 'gi')
    ->fields('gi')
    ->condition('iid', $iid)
    ->execute()
    ->fetch();

  $message = 'Edit invitation for %mail in %group';
  $replace = array(
    '%mail' => $data->mail,
    '%group' => $group->label(),
  );
  drupal_set_title(t($message, $replace), PASS_THROUGH);

  $roles = array();
  foreach ($group->getRoles(FALSE) as $group_role) {
    $roles[$group_role->name] = $group_role->label();
  }

  $form['roles'] = array(
    '#title' => t('Group roles'),
    '#type' => 'checkboxes',
    '#options' => $roles,
    '#default_value' => unserialize($data->roles),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit invitation'),
  );

  return $form;
}

/**
 * Group e-mail invite edit callback.
 */
function ginvite_by_mail_edit_form_submit($form, &$form_state) {
  list($group, $iid) = $form_state['build_info']['args'];

  $roles = array_values(array_filter($form_state['values']['roles']));
  db_update('group_invite')
    ->fields(array('roles' => serialize($roles)))
    ->condition('iid', $iid)
    ->execute();

  drupal_set_message(t('The invitation was successfully edited.'));
  $form_state['redirect'] = "group/$group->gid/invite/mail-list";
}

/**
 * Group e-mail invite deletion form.
 */
function ginvite_by_mail_delete_form($form, &$form_state, Group $group, $iid) {
  $data = db_select('group_invite', 'gi')
    ->fields('gi')
    ->condition('iid', $iid)
    ->execute()
    ->fetch();

  $message = t('Are you sure you want to delete the invitation for %mail?');
  $replace = array(
    '%mail' => $data->mail,
  );

  $redirect = "group/$group->gid/invite/mail-list";
  $description = t('Keep in mind the recipient has already received an e-mail.');

  return confirm_form($form, t($message, $replace), $redirect, $description);
}

/**
 * Group e-mail invite deletion callback.
 */
function ginvite_by_mail_delete_form_submit($form, &$form_state) {
  db_delete('group_invite')
    ->condition('iid', $form_state['build_info']['args'][1])
    ->execute();

  drupal_set_message(t('The invitation has been removed.'));
}
