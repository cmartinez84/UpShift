;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Checkall module for Drupal 6
;;
;; Original author: markus_petrux (http://drupal.org/user/39593)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CONTENTS
========
* OVERVIEW
* INSTALLATION
* USAGE
* CREDITS


OVERVIEW
========

This module provides an extension to the builtin checkboxes element that allows
developers add "check all" / "uncheck all" actions to them. This is specially
useful for checkboxes that have a lot of elements.

Please note that Drupal 6 already has this feature available, but only for
checkboxes rendered in tables, such as those used in Administration -> Content
management -> Content. Checkboxes rendered elsewhere do not have this feature.

You may not need to install this module unless it is required/recommended by
another contributed module, and/or you need it for your own custom modules.


INSTALLATION
============

- Copy all contents of this package to your modules directory preserving
  subdirectory structure.

- Goto Administer > Site building > Modules to install this module.


USAGE
=====

To attach the checkall behavior to checkboxes elements, all you need to do
is add the '#checkall' property to them. You can do so for your own forms, or
you can implement hook_alter_form() to add this property to existing forms.

Example:

  $form['foobar'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $default_value,
    '#checkall' => TRUE,  // <--- this is it ;-)
  );

This will add all three links (check all, toggle, and uncheck all) to the form
element.  If you want to disable one of the links, you can add a property for
each item:

Example:
  $form['foobar'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $default_value,
    '#checkall' => TRUE,
    '#checkall-toggle' => FALSE,
    '#checkall-check-all' => TRUE,
    '#checkall-uncheck-all' => TRUE,
  );

This will add the check all and uncheck all, but not the toggle links.  Any of
the three not specified will default to TRUE.

CREDITS
=======

The first version of the module was written by rszrama [1] and it is still
available from the Drupal 5 branch of the project.

Many thanks to him for letting markus_petrux [2] use his project for the
implementation of this checkboxes addon for Drupal 6.

Peter Lieverdink [3] and John Franklin [4] then picked it up for the Drupal 7
port.

[1] https://www.drupal.org/user/49344
[2] https://www.drupal.org/user/39593
[3] https://www.drupal.org/u/cafuego
[4] https://www.drupal.org/u/John-Franklin