<?php

/**
 * @file
 * Provides tokens by the shoutbox_message data type.
 */

/**
 * Implements hook_token_info().
 */
function shoutbox_rules_token_info() {
  $type = array(
    'name' => t('Shoutbox message'),
    'description' => t('Tokens related to individual shoutbox messages.'),
    'needs-data' => 'shoutbox_message',
  );

  $shoutbox_message['shoutbox-message-id'] = array(
    'name' => t('Shoutbox message ID'),
    'description' => t('Shoutbox message ID'),
  );
  $shoutbox_message['shoutbox-message-body'] = array(
    'name' => t('Body of the Shoutbox message'),
    'description' => t('Body of the Shoutbox message'),
  );
  $shoutbox_message['shoutbox-message-body-raw'] = array(
    'name' => t('Body of the Shoutbox message. WARNING - raw user input'),
    'description' => t('Body of the Shoutbox message. WARNING - raw user input'),
  );

  return array(
    'types' => array('shoutbox_message' => $type),
    'tokens' => array('shoutbox_message' => $shoutbox_message),
  );
}

/**
 * Implements hook_tokens().
 */
function shoutbox_rules_tokens($type, $tokens, array $data = array(), array $options = array()) {

  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  if ($type == 'shoutbox_message' && !empty($data['shoutbox_message'])) {
    $shoutbox_message = $data['shoutbox_message'];

    foreach ($tokens as $name => $original) {
      switch ($name) {

        case 'shoutbox-message-id':
          $replacements[$original] = check_plain($shoutbox_message->shout_id);
          break;
        case 'shoutbox-message-body':
          $replacements[$original] = check_plain($shoutbox_message->shout);
          break;
        case 'shoutbox-message-body-raw':
          $replacements[$original] = $shoutbox_message->shout;
          break;
      }
    }
  }

  return $replacements;
}
