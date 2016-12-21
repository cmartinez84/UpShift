<?php

/**
 * @file
 * Contains UserRelationshipsController
 *
 * This is some backwards stuff, the entity controller will be implementing the
 * CRUD methods laid out by user_relationships.module
 */

/**
 * UserRelationshipsController
 * The Entity controller for a relationship.
 */
class UserRelationshipsController extends EntityAPIController {
  public $account;
  public $reason;

  /**
   * Overridden.
   * @see EntityAPIController#__construct()
   */
  public function __construct($entityType = 'user_relationships', $account = NULL) {
    parent::__construct($entityType);

    if (!$account) {
      global $user;
      $account = $user->uid ? user_load($user->uid) : NULL;
    }
    $this->setAccount($account);

    // We force set the bundle key as the user relationship type whilst
    // exportable isn't a full entity type.
    //$this->bundleKey = 'type';
  }

  /**
   * Overrides EntityAPIController::load().
   *
   * When loading a relationship make sure the type machine name is present.
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = parent::load($ids, $conditions);

    if (!empty($entities)) {
      $types = user_relationships_types_load();
      foreach ($entities as $id => $entity) {
        $entities[$id]->type = $types[$entity->rtid]->machine_name;
      }
    }

    return $entities;
  }

  /**
   * Set the current user account.
   */
  public function setAccount($account) {
    $this->account = $account;
  }

  /**
   * Get the current user account.
   */
  public function getAccount() {
    return $this->account;
  }

  /**
   * Set the reason for deletion.
   */
  public function setReason($reason) {
    $this->reason = $reason;
  }

  /**
   * Get the reason.
   */
  public function getReason() {
    return $this->reason;
  }

  /**
   * Overrides EntityAPIController::buildQuery().
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);

    // Load queries will be from the current users perspective.
    $account = $this->getAccount();
    if ($account) {
      $query->condition('requester_id', $account->uid)
        ->addMetaData('account', $account);
    }

    return $query;
  }

  /**
   * Request a new relationship with another user.
   */
  public function request($target_user, $type_name, $source_user = NULL) {
    $type = user_relationships_type_load(array('machine_name' => $type_name));
    if (!$type) {
      throw new Exception(t('No such relationship type.'));
    }

    if (!$source_user) {
      $source_user = $this->getAccount();

      if (!$source_user) {
        global $user;

        if (!$user->uid) {
          return NULL;
        }

        user_load($user->uid);
      }
    }

    // If there is already an existing relationship, return it.
    if (!$type->is_oneway || !$type->is_reciprocal) {
      $existing_relationship = user_relationships_load(array('rtid' => $type->rtid, 'between' => array($source_user->uid, $target_user)));
    }
    else {
      $existing_relationship = user_relationships_load(array('rtid' => $type->rtid, 'requester_id' => $source_user->uid, 'requestee_id' => $target_user));
    }

    if (!empty($existing_relationship)) {
      return current($existing_relationship);
    }

    $ret = user_relationships_request_relationship($source_user, $target_user, $type);
    if (!$ret) {
      throw new Exception(t('Unknown failure or permission denied'));
    }
    elseif (!is_object($ret)) {
      throw new Exception($ret);
    }

    // Force set the type.
    db_update('user_relationships')
      ->fields(array(
        'type' => $type_name,
      ))
      ->condition('rid', $ret->rid)
      ->execute();
    $ret->type = $type_name;

    return $ret;
  }

  /**
   * Accept/approve a relationship.
   */
  function accept($rid, $account = NULL) {
    if (!$account) {
      $account = $this->getAccount();

      if (!$account) {
        return NULL;
      }
    }

    $rels = user_relationships_load(array('rid' => $rid , 'requestee_id' => $account->uid, 'approved' => 0));
    if (!$rels || !is_array($rels) || count($rels) != 1) {
      throw new Exception('User relationship load failed');
    }

    $rel = array_shift($rels);
    if ($rel->requestee_id != $account->uid) {
      throw new Exception('Access denied');
    }

    user_relationships_save_relationship($rel, 'approve');
    return $rel;
  }

  /**
   * Overridden.
   * @see EntityAPIController#delete()
   */
  public function delete($ids) {
    $account = $this->getAccount();
    $reason = $this->getReason();

    if (empty($ids) || !$account || !$reason) {
      return FALSE;
    }

    // Do it.
    foreach ($ids as $id) {
      $rels = user_relationships_load(array('rid' => $id , 'user' => $account->uid));
      if (!$rels || !is_array($rels) || count($rels) != 1) {
        throw new Exception('User relationship load failed');
      }

      $rel = array_shift($rels);
      if ($rel->requestee_id != $account->uid && $rel->requester_id != $account->uid) {
        throw new Exception('Access denied');
      }

      user_relationships_delete_relationship($rel, $account, $reason);
    }
  }
}
