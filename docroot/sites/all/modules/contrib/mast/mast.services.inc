<?php
/**
 * @file
 * Service module resource callbacks for the MASt module.
 */

/**
 * Page callback; returns information on the available MASt API endpoints.
 */
function mast_api_available() {
  $mast_bundle_settings = variable_get('mast_bundle_settings', array());
  $response = array('available' => FALSE);

  foreach ($mast_bundle_settings as $entity_type => $bundles) {
    $response['entity_enabled_bundles'][$entity_type] = 0;
    foreach ($bundles as $bundle_name => $settings) {
      if ($settings['enabled']) {
        $response['available'] = TRUE;
        $response['entity_enabled_bundles'][$entity_type]++;
      }
    }
    $response['entity_enabled_bundles'] = array_filter($response['entity_enabled_bundles']);
  }
  $listed_resources = array(
    'fields' => 'fields',
    'meta' => 'meta',
    'content-by-user' => 'content-by-user',
    'field-instance-file' => 'field-instance-file',
    'node' => 'content',
  );
  if ($response['available']) {
    $endpoint = services_endpoint_load('mast');
    $path = $endpoint->path . '/';
    foreach ($listed_resources as $name => $mast_name) {
      $response['resources'][$mast_name] = $path;
      $response['resources'][$mast_name] .= isset($endpoint->resources[$name]['alias']) ? $endpoint->resources[$name]['alias'] : $name;
    }
  }
  // We are returning JSON, so tell the browser.
  drupal_add_http_header('Content-Type', 'application/json');
  echo json_encode($response);
}


/**
 * Implements hook_mast_classes().
 */
function mast_mast_classes() {
  return array(
    'entities' => array(
      'node' => 'mastEntityStructureNode',
    ),
    'field_types' => array(
      'text_with_summary' => 'mastFieldFormattedText',
      'image' => 'mastImageField',
      'taxonomy_term_reference' => 'mastTaxonomyTermReferenceField',
      'user_reference' => 'mastUserReferenceField',
      'list_text' => 'mastListField',
      'list_integer' => 'mastListField',
      'list_float' => 'mastListField',
      'list_boolean' => 'mastListBooleanField',
      'media' => 'mastMediaField',
      'social_publish_toggle' => 'mastToggleField',
      'feature_toggle' => 'mastToggleField',
      'datetime' => 'mastDateField',
      'date' => 'mastDateField',
      'datestamp' => 'mastDateField',
    ),
    'fields' => array(
      'author' => 'mastAuthorField',
    ),
  );
}

/**
 * Implements hook_mast_field_types().
 */
function mast_mast_field_types() {
  return array(
    'text',
    'text_long',
    'text_with_summary',
    'list_integer',
    'list_float',
    'list_boolean',
    'list_text',
    'image',
    'taxonomy_term_reference',
    'number_integer',
    'number_decimal',
    'number_float',
    'datetime',
    'date',
    'datestamp',
    'media',
    'user_reference',
    'social_publish_toggle',
    'feature_toggle',
  );
  // @todo -  add 'file' field support if there are apps/devices that can use it.
}

/**
 * Helper function for mast_services_resources().
 */
function _mast_autocomplete_resource($callback = '', $resources_file = '') {
return array(
    'index' => array(
      'help' => 'Autocomplete based on leading string for a given type',
      'callback' => $callback,
      'access callback' => 'user_access',
      'access arguments' => array('access content'),
      'access arguments append' => FALSE,
      'file' => $resources_file,
      'args' => array(
        array(
          'name' => 'field_name',
          'type' => 'string',
          'description' => 'The field name',
          'source' => array('param' => 'field_name'),
          'optional' => FALSE,
        ),
        array(
          'name' => 'bundle',
          'type' => 'string',
          'description' => 'The bundle name',
          'source' => array('param' => 'bundle'),
          'optional' => FALSE,
        ),
        array(
          'name' => 'match',
          'type' => 'string',
          'description' => 'The leading string to match against',
          'source' => array('param' => 'match'),
          'optional' => FALSE,
        ),
        array(
          'name' => 'count',
          'type' => 'int',
          'description' => 'The number of results to return.',
          'default value' => 10,
          'source' => array('param' => 'count'),
          'optional' => TRUE,
        ),
      ),
    ),
  );
}

/**
 * Helper function for mast_services_resources().
 */
function _mast_options_resource($callback = '', $resources_file = '') {
return array(
    'index' => array(
      'help' => 'Retrieve a page of options for a given type',
      'callback' => $callback,
      'access callback' => 'user_access',
      'access arguments' => array('access content'),
      'access arguments append' => FALSE,
      'file' => $resources_file,
      'args' => array(
        array(
          'name' => 'field_name',
          'type' => 'string',
          'description' => 'The field name',
          'source' => array('param' => 'field_name'),
          'optional' => FALSE,
        ),
        array(
          'name' => 'bundle',
          'type' => 'string',
          'description' => 'The bundle name',
          'source' => array('param' => 'bundle'),
          'optional' => FALSE,
        ),
        array(
          'name' => 'start',
          'type' => 'int',
          'description' => 'The start of results to return.',
          'default value' => 0,
          'source' => array('param' => 'start'),
          'optional' => TRUE,
        ),
        array(
          'name' => 'count',
          'type' => 'int',
          'description' => 'The number of results to return.',
          'default value' => 10,
          'source' => array('param' => 'count'),
          'optional' => TRUE,
        ),
      ),
    ),
  );
}

/**
 * Implements hook_services_resources().
 *
 * Provide resources for content type information.
 */
function mast_services_resources() {
  $resources_file = array('type' => 'inc', 'module' => 'mast', 'name' => 'mast.services');

  $res['autocomplete'] = _mast_autocomplete_resource('_mast_autocomplete_node', $resources_file);
  $res['options'] = _mast_options_resource('_mast_options_node', $resources_file);

  $res['fields'] = array(
    'index' => array(
      'help' => 'Retrieves a listing of eligible content types',
      'callback' => '_mast_fields_index',
      'access callback' => 'user_access',
      'access arguments' => array('access content'),
      'access arguments append' => FALSE,
      'file' => $resources_file,
    ),
  );
  $res['meta'] = array(
    'index' => array(
      'help' => 'Retrieves a listing of available meta data',
      'callback' => '_mast_meta_index',
      'access callback' => 'user_access',
      'access arguments' => array('access content'),
      'access arguments append' => FALSE,
      'file' => $resources_file,
    ),
    'retrieve' => array(
      'help' => 'Retrieves a listing of available meta data',
      'callback' => '_mast_meta_retrieve',
      'access callback' => 'user_access',
      'access arguments' => array('access content'),
      'access arguments append' => FALSE,
      'file' => $resources_file,
      'args' => array(
        array(
          'name' => 'type',
          'optional' => FALSE,
          'source' => array('path' => 0),
          'type' => 'string',
          'description' => 'The type of the meta data to get',
        ),
      ),
    ),
  );
  $res['content-by-user'] = array(
    'index' => array(
      'help' => 'Retrieves a listing of content by the current user',
      'callback' => '_mast_content_by_user_index',
      'access callback' => 'user_access',
      'access arguments' => array('access content'),
      'access arguments append' => FALSE,
      'file' => $resources_file,
      'args' => array(
        array(
          'name' => 'start',
          'optional' => TRUE,
          'type' => 'int',
          'description' => 'The zero-based index of the page to get, defaults to 0.',
          'default value' => 0,
          'source' => array('param' => 'start'),
        ),
        array(
          'name' => 'count',
          'optional' => TRUE,
          'type' => 'init',
          'description' => 'Number of records to get per page.',
          'default value' => variable_get('services_node_index_page_size', 20),
          'source' => array('param' => 'count'),
        ),
      ),
    ),
    'create' => array(
      'help' => 'Creates a node',
      'file' => $resources_file,
      'callback' => '_mast_content_by_user_create',
      'access callback' => '_mast_node_resource_access',
      'access arguments' => array('create'),
      'access arguments append' => TRUE,
        'args' => array(
           array(
             'name' => 'data',
             'type' => 'struct',
             'description' => 'The node object',
             'source' => 'data',
             'optional' => FALSE,
           ),
         ),
       ),
    );
  $res['field-instance-file'] = array(
      'create' => array(
        'help' => 'Creates a file with raw data.',
        'file' => $resources_file,
        'callback' => '_mast_field_instance_file_create',
        'access callback' => '_mast_file_resource_access',
        'access arguments' => array('create_raw'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'file',
            'type' => 'array',
            'description'    => t('An array of instance info for the posted file.'),
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),
    );
  return $res;
}

/**
 * Create an endpoint to retrieve content type information.
 */
function _mast_fields_index() {
  $classes = mastFactory::registry();

  // For nodes we may need to limit the number of enabled bundles, depending on
  // how this setting has been configured.
  $count = 0;
  $max_allowed_node_types = variable_get('mast_max_allowed_node_types', MAST_MAX_ALLOWED_NODE_TYPES);

  $mast_bundle_settings = variable_get('mast_bundle_settings', array());
  $allowed_types = array();
  foreach ($mast_bundle_settings as $entity_type => $bundles) {
    foreach ($bundles as $bundle_name => $settings) {
      if ($settings['enabled']) {
        $mast_structure = mastFactory::createEntityStructure($entity_type, $bundle_name);
        if ($mast_structure->createPermitted($entity_type, $bundle_name)) {
          $allowed_types[$entity_type][$bundle_name] = array(
            'short_name' => $settings['short_name'],
          );
          if (!empty($settings['icon_uri'])) {
            $allowed_types[$entity_type][$bundle_name]['icon_url'] = file_create_url($settings['icon_uri']);
          }
          elseif (!empty($settings['icon'])) {
            $allowed_types[$entity_type][$bundle_name]['icon'] = $settings['icon'];
          }
          $allowed_types[$entity_type][$bundle_name]['fields'] = $mast_structure->getAllFields();
          if ($entity_type == 'node' && $max_allowed_node_types) {
            $count++;
            if ($count >= $max_allowed_node_types) {
              break; // break out of the node bundles loop
            }
          }
        }
      }
    }
  }
  return $allowed_types;
}

/**
 * Helper for resource callbacks - get field object for a file name and bundle.
 *
 * @throws mastParamsException
 */
function mast_node_resources_get_field($field_name, $bundle_name) {
  $mast_bundle_settings = variable_get('mast_bundle_settings', array());
  if (empty($mast_bundle_settings['node'][$bundle_name]['enabled'])) {
    throw new mastParamsException(t('Content type @bundle_name is not enable for MASt', array('@bundle_name' => $bundle_name)));
  }
  $field_info = mast_field_info_field($field_name);
  $instance_info = mast_field_info_instance('node', $field_name, $bundle_name);
  if (empty($field_info) || empty($instance_info)) {
    throw new mastParamsException(t('Field @field_name for bundle @bundle_name not found.', array('@field_name' => $field_name, '@bundle_name' => $bundle_name)));
  }
  return mastFactory::createFieldStructure($field_info, $instance_info);
}

/**
 * Provide a listing of allowed options for a field.
 */
function _mast_options_node($field_name, $bundle_name, $start, $count) {
  try {
    $field = mast_node_resources_get_field($field_name, $bundle_name);
    return $field->options($start, $count);
  }
  catch (mastParamsException $e) {
    return services_error((string) $e->getMessage(), 400);
  }
}

/**
 * Match allowed options for a field.
 */
function _mast_autocomplete_node($field_name, $bundle_name, $match, $count) {
  try {
    $field = mast_node_resources_get_field($field_name, $bundle_name);
    return $field->autoComplete($match, $count);
  }
  catch (mastParamsException $e) {
    return services_error((string) $e->getMessage(), 400);
  }
}

/**
 * Creates a node using the received data and returns the nid.
 */
function _mast_content_by_user_create($data) {
  // @todo Figure out the correct way to be passing the node data.
  $data = json_decode($data['node'], TRUE);
  if (!isset($data['type'])) {
    echo t('Node type not specified');
    return services_error(t('Node type not specified'), 422);
  }
  $mast_structure = mastFactory::createEntityStructure('node', $data['type']);
  try {
    $node_data = $mast_structure->prepareValues($data);
  }
  catch (MastException $e) {
    echo $e->getMessage();
    return services_error($e->getMessage(), 422);
  }
  try {
    $wrapper = entity_property_values_create_entity('node', $node_data);
  }
  catch (EntityMetadataWrapperException $e) {
    echo $e->getMessage();
    return services_error($e->getMessage(), 422);
  }

  $wrapper->save();
  return array('nid' => $wrapper->getIdentifier());
}

function _mast_meta_index() {
  return array(
    'last' => TRUE,
    'values' => array(
      "whoami" => "Authenticated user info",
      "site" => "Site URL, name and icon",
    ),
  );
}

function _mast_meta_retrieve($type) {
  $valid = _mast_meta_index();
  switch ($type) {
    case "whoami":
      global $user;
      return array(
        'id' => $user->uid,
        'name' => $user->name,
      );
    case "site":
      global $user;
      return array(
        'id' => url('<front>', array('absolute' => TRUE)),
        'name' => variable_get('site_name', 'Drupal'),
        'icon' => mast_get_favicon(),
      );
    default:
      return NULL;
  }
}

/**
 * Return an array of paged nodes by the authenticated user.
 *
 * An example request might look like
 *
 * http://domain/endpoint/content-by-user?start=20&count=10
 *
 * @param $start
 *   The offset into the result set.
 * @param $count
 *   Integer number of items to be returned.
 * @return
 *   An array of node teasers.
 */
function _mast_content_by_user_index($start, $count) {
  global $user;
  $mast_bundle_settings = variable_get('mast_bundle_settings', array('node' => array()));

  $bundles = array();
  $mast_structure = array();
  foreach ($mast_bundle_settings['node'] as $bundle => $settings) {
    if ($settings['enabled']) {
      $bundles[] = $bundle;
      $mast_structure[$bundle] = mastFactory::createEntityStructure('node', $bundle);
    }
  }
  if (empty($bundles)) {
    $data = array('start' => $start);
    $data['last'] = TRUE;
    $data['values'] = new stdClass();
    return $data;
  }

  $query = db_select('node', 'n', array('fetch' => PDO::FETCH_ASSOC));
  $query->fields('n', array('nid'));
  $query->condition('n.status', 1);
  $query->condition('n.type', $bundles, 'IN');

  // If using the Original Author module to keep track of who created each node,
  // regardless of attributed author, add an "OR" condition that checks the node
  // uid and the uid column of the original_author table.
  if (module_exists('original_author')) {
    $o_alias = $query->leftJoin('original_author', 'o', '%alias.nid = n.nid');
    $or = db_or()
      ->condition('n.uid', $user->uid)
      ->condition("$o_alias.uid", $user->uid);
    $query->condition($or);
  }
  else {
    $query->condition('n.uid', $user->uid);
  }

  $query->addTag('node_access');
  $query->addTag('mast_content_by_user');

  $count_query = clone($query);
  $count_query = $count_query->countQuery();
  $query->range($start, $count);
  $query->orderBy('n.created', 'DESC');

  try {
    $total = $count_query->execute()->fetchField();
    $results = $query->execute()->fetchCol();
  }
  catch (Exception $e) {
    $msg = t('Invalid query provided, double check that the parameters you defined are correct and exist. @message', array('@message' => $e->getMessage()));
    echo $msg;
    return services_error($msg, 422);
  }

  $values = array();
  $nodes = node_load_multiple($results);
  foreach ($nodes as $id => $node) {
    $values[$id] = array(
      'type' => $node->type,
      'title' => $node->title,
      'teaser' => $mast_structure[$node->type]->getTeaser($node),
      'image' => $mast_structure[$node->type]->getImage($node),
      'timestamp' => $node->created,
      'path' => drupal_get_path_alias("node/{$id}"),
    );
  }
  $data = array('start' => $start);
  if ($total <= ($start + $count)) {
    $data['last'] = TRUE;
  }
  $data['values'] = $values;
  return $data;
}

/**
 * Creates a file from a raw file upload, validating for a particular field instance.
 *
 * @param $data
 *   An array of information describing the field insntace. Keys:
 *     - (required) 'bundle' The name of the bundle the field is attached to.
 *     - (required) 'field_name' The name of the Field API field.
 *     - (optional) 'entity_type' Defaults to 'node'.
 *
 * @return An array of arrays representing successfully saved files, each with
 *   just the 'fid' key specified.
 */
function _mast_field_instance_file_create($data) {
  // The data array should contain information about the field instance that the
  // posted file is being uploaded for. This allows us to validate it against
  // the instance settings.
  if (!isset($data['bundle']) || !isset($data['field_name'])) {
    $msg = t('Cannot validate file - bundle or field name was not provided');
    echo $msg;
    return services_error($msg, 422);
  }
  $entity_type = isset($data['entity_type']) ? $data['entity_type'] : 'node';

  $instance = field_info_instance($entity_type, $data['field_name'], $data['bundle']);
  $settings = $instance['settings'];

  $max_filesize = parse_size(file_upload_max_size());
  if (!empty($settings['max_filesize']) && parse_size($settings['max_filesize']) < $max_filesize) {
    $max_filesize = parse_size($settings['max_filesize']);
  }

  $validators = array(
    'file_validate_size' => array($max_filesize),
    'file_validate_extensions' => array($settings['file_extensions'])
  );
  if (isset($settings['max_resolution']) && isset($settings['min_resolution'])) {
    $validators['file_validate_image_resolution'] = array($settings['max_resolution'], $settings['min_resolution']);
  }

  if (!is_array($_FILES['files']['name'])) {
    // Restructure the $_FILES array so that it corresponds to what
    // file_save_upload() is expecting.
    foreach ($_FILES['files'] as $key => &$value) {
      $value = array($data['field_name'] => $value);
    }
  }
  if ($file = file_save_upload($data['field_name'], $validators, file_default_scheme() . "://")) {
    if ($file->fid) {
      file_usage_add($file, 'mast', 'files', $file->fid);
      return array('fid' => $file->fid);
    }
    else {
      return services_error(t('An unknown error occured'), 500);
    }
  }
  return services_error(t('Could not upload file'), 500);
}

/**
 * Wrapper around node_access()
 */
function _mast_node_resource_access($op = 'view', $args = array()) {
  if (isset($args[0]['node'])) {
    $node = json_decode($args[0]['node']);
    if (!isset($node->nid) && $op == 'create') {
      $node = $node->type;
    }
    return node_access($op, $node);
  }
  return FALSE;
}

/**
 * Wrapper around node_access() for files
 */
function _mast_file_resource_access($op = 'view', $args = array()) {
  if ($op == 'create_raw') {
    $data = $args[0];
    if (!isset($data['entity_type']) || $data['entity_type'] == 'node') {
      if (!isset($data['bundle'])) {
        return FALSE;
      }
      return node_access('create', $data['bundle']);
    }
    // @todo Support other entity types?
  }
  return FALSE;
}

/**
 * Returns the site's favicon URL and mimetype.
 *
 * @return array
 *   An associative array:
 *   - icon: The URL to the favicon.
 *   - type: The favicon's mimetype.
 */
function mast_get_favicon() {
  // The favicon is added in template_preprocess_html(). Force a blank page
  // to render, so that we invoke the entire theme system.
  drupal_render_page('');
  // The favicon can now be found as an html head tag. Retrieve it and allow
  // alter hooks to act on it.
  $elements = drupal_add_html_head();
  drupal_alter('html_head', $elements);
  // Pull the favicon out of the list of html head tags and retrieve its
  // URL and mimetype.
  foreach ($elements as $element) {
    if (isset($element['#attributes']['rel']) && $element['#attributes']['rel'] === 'shortcut icon') {
      return array(
        'icon' => $element['#attributes']['href'],
        'type' => $element['#attributes']['type'],
      );
    }
  }
}

/**** Ctools plugin definitions ****/

/**
 * Implements hook_default_services_endpoint().
 */
function mast_default_services_endpoint() {
  $endpoints = array();
  $endpoint = new stdClass;
  $endpoint->disabled = FALSE; /* Edit this to true to make a default endpoint disabled initially */
  $endpoint->api_version = 3;
  $endpoint->name = 'mast';
  $endpoint->server = 'rest_server';
  $endpoint->path = 'mast-api';
  $endpoint->authentication = array(
    'services_oauth' => array(
      'oauth_context' => 'mast',
      'credentials' => 'token',
    ),
  );
  $endpoint->server_settings = array(
    'formatters' => array(
      'json' => TRUE,
      'xml' => FALSE,
      'bencode' => FALSE,
      'jsonp' => FALSE,
      'php' => FALSE,
      'rss' => FALSE,
      'yaml' => FALSE,
    ),
    'parsers' => array(
      'application/json' => TRUE,
      'application/x-www-form-urlencoded' => TRUE,
      'multipart/form-data' => TRUE,
      'application/vnd.php.serialized' => FALSE,
      'application/x-yaml' => FALSE,
    ),
  );
  $endpoint->resources = array(
    'autocomplete' => array(
      'operations' => array(
        'index' => array(
          'enabled' => 1,
        ),
      ),
    ),
    'options' => array(
      'operations' => array(
        'index' => array(
          'enabled' => 1,
        ),
      ),
    ),
    'node' => array(
      'alias' => 'content',
      'operations' => array(
        'delete' => array(
          'enabled' => 1,
        ),
      ),
    ),
    'fields' => array(
      'operations' => array(
        'index' => array(
          'enabled' => 1,
        ),
      ),
    ),
    'meta' => array(
      'operations' => array(
        'index' => array(
          'enabled' => 1,
        ),
        'retrieve' => array(
          'enabled' => 1,
        ),
      ),
    ),
    'content-by-user' => array(
      'operations' => array(
        'index' => array(
          'enabled' => 1,
        ),
        'create' => array(
          'enabled' => 1,
        ),
      ),
    ),
    'field-instance-file' => array(
      'operations' => array(
        'create' => array(
          'enabled' => 1,
        ),
      ),
    ),
    'user' => array(
      'actions' => array(
        'logout' => array(
          'enabled' => 1,
        ),
      ),
    ),
  );
  $endpoint->debug = 0;
  $endpoints['mast_content_types_endpoint'] = $endpoint;
  return $endpoints;
}
