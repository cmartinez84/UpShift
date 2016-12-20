<?php
/**
 * @file
 * Contains all router functions for the Group invite module.
 */

/**
 * Implements hook_menu().
 */
function ginvite_menu() {
  $items['group/%group/invite'] = array(
    'title' => 'Invite member',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ginvite_by_user_form', 1),
    'access callback' => 'group_access',
    'access arguments' => array('invite members by user', 1),
    'file' => 'forms/ginvite.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['group/%group/invite/user'] = array(
    'title' => 'By username',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );

  $items['group/%group/invite/mail'] = array(
    'title' => 'By e-mail',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ginvite_by_mail_form', 1),
    'access callback' => 'group_access',
    'access arguments' => array('invite members by mail', 1),
    'file' => 'forms/ginvite.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['group/%group/invite/mail/%/edit'] = array(
    'title' => 'Edit invitation',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ginvite_by_mail_edit_form', 1, 4),
    'access callback' => 'group_access',
    'access arguments' => array('manage mail invitations', 1),
    'file' => 'admin/ginvite.inc',
    'type' => MENU_CALLBACK,
  );

  $items['group/%group/invite/mail/%/delete'] = array(
    'title' => 'Delete invitation',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ginvite_by_mail_delete_form', 1, 4),
    'access callback' => 'group_access',
    'access arguments' => array('manage mail invitations', 1),
    'file' => 'admin/ginvite.inc',
    'type' => MENU_CALLBACK,
  );

  $items['group/%group/invite/mail-list'] = array(
    'title' => 'E-mail invitations',
    'page callback' => 'ginvite_by_mail_list',
    'page arguments' => array(1),
    'access callback' => 'group_access',
    'access arguments' => array('manage mail invitations', 1),
    'file' => 'admin/ginvite.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 10,
  );

  $items['user/%user/group/invite'] = array(
    'title' => 'My invites',
    'title callback' => 'ginvite_user_page_title',
    'title arguments' => array(1),
    'description' => 'Group invitations for a given user.',
    'page callback' => 'ginvite_user_page',
    'page arguments' => array(1),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/ginvite.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $items['user/%user/group/invite/%group/accept'] = array(
    'page callback' => 'ginvite_accept_callback',
    'page arguments' => array(1, 4),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/ginvite.inc',
    'type' => MENU_CALLBACK,
  );

  $items['user/%user/group/invite/%group/decline'] = array(
    'page callback' => 'ginvite_decline_callback',
    'page arguments' => array(1, 4),
    'access callback' => 'user_edit_access',
    'access arguments' => array(1),
    'file' => 'pages/ginvite.inc',
    'type' => MENU_CALLBACK,
  );

  return $items;
}

/**
 * Title callback for user/%user/group/invite.
 */
function ginvite_user_page_title($account) {
  global $user;

  if ($user->uid == $account->uid) {
    return t('My invites');
  }

  return t("@user's invites", array('@user' => format_username($account)));
}
