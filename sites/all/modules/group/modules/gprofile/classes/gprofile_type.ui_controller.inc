<?php
/**
 * @file
 * Defines the Entity API UI class for group member profile types.
 */

/**
 * UI class for group member profile types.
 */
class GProfileTypeUIController extends EntityDefaultUIController {

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    // Unset admin/group/member paths as they are already in use by Group.
    unset($items[$this->path], $items["$this->path/list"]);

    return $items;
  }

}
