<?php
/**
 * @file
 * Defines the Entity API class for group member profiles.
 */

/**
 * Main class for group member profiles.
 */
class GProfile extends Entity {

  /**
   * Defines the entity label if the 'entity_class_label' callback is used.
   */
  protected function defaultLabel() {
    $group_membership = $this->getMember();

    $message = '%profile profile for %member in %group';
    $replace = array(
      '%profile' => gprofile_type_load($this->type)->label(),
      '%member' => format_username(user_load($group_membership->uid)),
      '%group' => group_load($group_membership->gid)->label(),
    );

    return t($message, $replace);
  }

  /**
   * Specifies the default uri, which is picked up by uri() by default.
   */
  protected function defaultURI() {
    return $this->getMember()->defaultURI();
  }

  /**
   * Get the member this profile belongs to.
   *
   * @return GroupMembership
   *   The GroupMembership entity of the member.
   */
  public function getMember() {
    $group_memberships = group_membership_load_multiple(array($this->mid));
    return reset($group_memberships);
  }
}
