<?php

/**
 * @file 
 * 	Page callbacks for entity devel.
 */

/**
 * Menu callback for devel render object.
 */
function devel_entity_render_object($entity_type, $entity, $name) {
  module_load_include('inc', 'entity', 'includes/entity.form');
  $build = $entity->view();
  return kdevel_print_object($build, '$' . $name . '->');
}

/**
 * Menu callback for devel render object.
 */
function devel_entity_load_object($entity_type, $entity, $name) {
  module_load_include('inc', 'entity', 'includes/entity.form');
  $build = $entity;
  return kdevel_print_object($build, '$' . $name . '->');
}
