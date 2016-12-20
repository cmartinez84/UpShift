<?php
/**
 * @file
 * Defines the Entity API extra fields class for group member profiles.
 */

/**
 * Controller for generating extra fields for group member profiles.
 */
class GProfileExtraFieldsController extends EntityDefaultExtraFieldsController {

  /**
   * Implements EntityExtraFieldsControllerInterface::fieldExtraFields().
   */
  public function fieldExtraFields() {
    $extra = array();

    foreach (gprofile_types() as $name => $gprofile_type) {
      // Get the redirect link for the edit and delete action.
      $options = array('query' => drupal_get_destination());

      // Add an extra field and display for every member profile.
      foreach (array('form', 'display') as $context) {
        $extra['group_membership']['group_membership'][$context]["gprofile_$name"] = array(
          'label' => $gprofile_type->label(),
          'description' => t('The %profile profile fields', array('%profile' => $gprofile_type->label())),
          'weight' => 86,
          'edit' => l(t('edit'), "admin/group/member/manage/$name", $options),
          'delete' => l(t('delete'), "admin/group/member/manage/$name/delete", $options),
        );
      }
    }

    return $extra;
  }
}
