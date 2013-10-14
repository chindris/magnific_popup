<?php

/**
 * @file magnific_popup/includes/magnific_popup.formatters.inc
 *
 * Formatters for Magnific Popup.
 */

/**
 * Implements hook_field_formatter_info().
 */
function magnific_popup_field_formatter_info() {
  $gallery_options = _magnific_popup_gallery_options();
  return array(
    'magnific_popup_file_field_formatter' => array(
      'label' => t('Magnific Popup'),
      'field types' => array('file', 'image'),
      'settings'  => array(
        'gallery_style' => key($gallery_options),
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function magnific_popup_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  
  // Gallery Mode is TRUE when this field supports multiple items.
  $gallery_available = $field['cardinality'] !== 1;
  $gallery_options = _magnific_popup_gallery_options();
  
  $element = array();
  $element['gallery_available'] = array(
    '#type' => 'markup',
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );
  $element['gallery_available']['#markup'] = $gallery_available ?
    t('This field configuration <strong>supports multiple items</strong> and can display itself as a gallery of items!') : 
    t('This field configuration <strong>does not support multiple items</strong> and cannot display itself as a gallery of items.');
  $element['gallery_style'] = array(
    '#type' => 'select',
    '#title' => t('Gallery Type'),
    '#options' => $gallery_options,
    '#disabled' => !$gallery_available,
    '#default_value' => $settings['gallery_style'],
    '#description' => t('Choose how this gallery displays its triggering element(s).'),
  );
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function magnific_popup_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $gallery_options = _magnific_popup_gallery_options();
  $summary = t('Gallery Type: @gallery_type', array('@gallery_type' => $gallery_options[$settings['gallery_style']]));
  return $summary;
}

/**
 * Implements hook_field_formatter_prepare_view().
 */
function magnific_popup_field_formatter_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items, $displays) {
  // Items are some kind of entity reference, be it via image or file type.
  // Load the corresponding items into a common format for our formatter_view().
  $loaded_items = &drupal_static(__FUNCTION__, array());
  // dpm($items);
  $supported_schemes = array(
    'youtube' => 'magnific_popup_yt_preview_wplay',
  );
  // Iterate through $items looking for valid gallery items. Refactor them into
  // a consistent format for magnific_popup_field_formatter_view().
  $wrappers = array();
  foreach ($entities as $entity_id => $entity) {
    foreach ($items[$entity_id] as $delta => &$item) {
      // Fail-safe check to not load items 
      if (empty($item['uri'])) {
        continue;
      }
      // Check if we've already parsed this fid, and build it if not.
      if (!isset($loaded_items[$item['fid']])) {
        $loaded_items[$item['fid']] = FALSE;
        $scheme = file_uri_scheme($item['uri']);
        // Attempt to re-use, or load from scratch, a wrapper for this scheme.
        if (empty($wrappers[$scheme])) {
          // Create a new wrapper.
          $wrappers[$scheme] = file_stream_wrapper_get_instance_by_uri($item['uri']);
        }
        else {
          // Attempt to re-use an existing wrapper.
          $wrappers[$scheme]->setUri($item['uri']);
        }
        // Check that the resource was accessible.
        if ($wrappers[$scheme] && $wrappers[$scheme]->url_stat($item['uri'], STREAM_URL_STAT_QUIET) !== FALSE) {
          $wrapper = $wrappers[$scheme];
          // Determine the file's info and store it.
          $thumbnail_path = FALSE;
          // MediaYouTubeStreamWrapper implements getLocalThumbnailPath() to
          // find the thumbnail path.
          if (method_exists($wrapper,'getLocalThumbnailPath')) {
            $thumbnail_path = $wrapper->getLocalThumbnailPath();
          }
          // Try the standard method to get a thumbnail.
          else {
            if ($uri = $wrapper->getUri()) {
              $thumbnail_path = $uri;
            }
          }
          // Adjust $thumbnail_path based on the $scheme - we support some
          // modules out of the box.
          // @todo Make the default thumbnail_style configurable.
          $thumbnail_style = 'thumbnail';
          if (!empty($supported_schemes[$scheme])) {
            $thumbnail_style = $supported_schemes[$scheme];
          }
          $thumbnail_path = image_style_url($thumbnail_style, $thumbnail_path);
          $gallery_item = array(
            'item' => $item,
            'title' => $item['filename'],
            'thumbnail_path' => $thumbnail_path,
            'target_uri' => $wrapper->getExternalUrl(),
          );
          $loaded_items[$item['fid']] = $gallery_item;
        }
      }
      // Replace $item with the parsed version of info for this fid.
      $item = $loaded_items[$item['fid']];
      // Check if this item was unloadable and if so remove it from the list.
      if (empty($item)) {
        unset($items[$entity_id][$delta]);
      }
    }
    // Remove the pointer.
    if (isset($item)) {
      unset($item);
    }
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function magnific_popup_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  if (!_magnific_popup_check_status()) {
    return FALSE;
  }
  // Add Magnific Popup library to page.
  drupal_add_library('magnific_popup', 'magnific_popup_iframe', FALSE);
  // drupal_add_js(drupal_get_path('module', 'magnific_popup') . '/js/behaviors.mfp-iframe.js');
  $element = array();
  $settings = $display['settings'];
  $popup_gallery = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(),
    ),
  );
  if ($settings['gallery_style'] != 'no_gallery') {
    $popup_gallery['#attributes']['class'][] = 'magnific-popup-iframe-gallery';
  }
  $trigger_item = key($items);
  if ($settings['gallery_style'] == 'random_item') {
    $trigger_item = array_rand($items);
  }
  foreach ($items as $delta => $item) {
    $popup_gallery['item-' . $delta] = array(
      '#theme' => 'link',
      '#text' => '',
      '#path' => $item['target_uri'],
      '#options' => array(
        'attributes' => array('class' => array(), 'style' => array()),
        'html' => TRUE,
      ),
    );
    $gallery_item_class = ($settings['gallery_style'] != 'no_gallery') ?
      'magnific-popup-iframe-gallery-item' : 'magnific-popup-no-gallery-item';
    $popup_gallery['item-' . $delta]['#options']['attributes']['class'][] = $gallery_item_class;
    if ($settings['gallery_style'] == 'all_items' || $settings['gallery_style'] == 'no_gallery' || $delta == $trigger_item) {
      $image = array(
        '#theme' => 'figure',
        '#path' => $item['thumbnail_path'],
        '#alt' => $item['title'],
        '#title' => $item['title'],
        // Let the images be fluid.
        '#attributes' => array(
          'style' => array(
            'max-width: 100%;',
            'height: auto;',
          ),
        ),
      );
      $popup_gallery['item-' . $delta]['#text'] = render($image);
    }
    else {
      $popup_gallery['item-' . $delta]['#options']['attributes']['style'][] = 'display: none;';
    }
  }
  if (count($items)) {
    $element[0] = $popup_gallery;
  }
  // dpm(array(
    // 'function' => __function__,
    // 'entity_type' => $entity_type,
    // 'entity' => $entity,
    // 'field' => $field,
    // 'instance' => $instance,
    // 'langcode' => $langcode,
    // 'items' => $items,
    // 'display' => $display,
  // ));
  return $element;
}

function _magnific_popup_gallery_options() {
  return array(
    'all_items' => t('Gallery: All Items Displayed'),
    'first_item' => t('Gallery: First Item Displayed'),
    'random_item' => t('Gallery: Random Item Displayed'),
    'no_gallery' => t('No Gallery: Display Each Item Separately'),
  );
}
