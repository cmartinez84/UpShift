<?php

/**
 * @file
 * Views integration for the shoutbox module
 */

/**
 * Implements hook_views_data()
 *
 */
function shoutbox_views_data() {
  $table = array(
    'shoutbox' => array(
      'table' => array(
        'group' => 'Shoutbox',
        'base' => array(
          'field' => 'shout_id',
          'title' => 'Soutbox shouts',
          'help' => 'Soutbox database table'
        ),
        'entity type' => 'shout',
      ),
      // fields
      //shout_id
      'shout_id' => array(
        'title' => t('Shout Id'),
        'help' => t('Soutbox table Id field'),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_numeric'
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      // uid
      'uid' => array(
        'title' => t('Shout UID'),
        'help' => t('Soutbox table UID field'),
        'field' => array(
          'handler' => 'views_handler_field_numeric',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_numeric'
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      // nick
      'nick' => array(
        'title' => t('The author\'s nickname.'),
        'help' => t('Shoutbox table Nick field'),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      // shout
      'shout' => array(
        'title' => t('Shout (message)'),
        'help' => t('Shoutbox table Shout field'),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
      // created
      'created' => array(
        'title' => t('Shout created'),
        'help' => t('Shoutbox table Created field'),
        'field' => array(
          'handler' => 'views_handler_field_date',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_date'
        ),
        'sort' => array(
          'handler' => 'views_handler_sort_date'
        )
      ),
      // changed
      'changed' => array(
        'title' => t('Shout changed'),
        'help' => t('Shoutbox table Changed field'),
        'field' => array(
          'handler' => 'views_handler_field_date',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_date'
        ),
        'sort' => array(
          'handler' => 'views_handler_sort_date'
        )
      ),
      // hostname
      'hostname' => array(
        'title' => t('Shout hostname'),
        'help' => t('Shoutbox table Hostname field'),
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string'
        ),
        'sort' => array(
          'handler' => 'views_handler_sort'
        )
      ),
    )
  );

  return $table;
}
