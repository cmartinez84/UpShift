
DESCRIPTION
-----------
This module is designed to associate group specific menus with the Group module.

This is inspired by OG Menu and adapted to work with the Group module.

REQUIREMENTS
------------
- Group module (http://drupal.org/project/group).
- Menu module.

INSTALLATION
------------
- Enable the module.
- Give "administer group menu" permission to the desired roles.
- Visit the group menu configuration page to configure at will.
- Enable group content types for use with Group Menu.

USAGE
-----
- Administrators can create Group menus through the group menu interface at
  group/[gid]/menus.
- Group members with the "administer group menu" permission can manage menus.
- "administer group menu" permission can be granted on global or group level.
- For group types, users can create an associated menu by checking
  "Enable menu for this group" on the node edit/creation form.
- Group menus won't show on the regular menu interface. They show up on
  admin/structure/groupmenu.
- Ability to hide Group Menu's from the block admin interface and on other
  places for some contrib modules.

NOTES
-----
Be aware that since menu administration forms are mostly duplicated, if a
contrib module adds functionality to menu administration forms without
additional permissions, these additions may be available for Group menu users
with 'administer group menu' permission. This could allow these users to be able
to do things you don't want them to. Please report these modules if you catch
one.

TODO/BUGS/FEATURE REQUESTS
--------------------------
- Please test the D7 release and report any bugs or suggestions you might find.


CREDITS
-------
Authored and maintained by Mike Davis (mike.davis).
