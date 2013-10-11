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
  return array(
    'magnific_popup_file_field_formatter' => array(
      'label' => t('Magnific Popup'),
      'field types' => array('file'),
      'settings'  => array(),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function magnific_popup_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  return array();
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function magnific_popup_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = t('Placeholder', array());
  return $summary;
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
  switch ($display['type']) {
    case 'magnific_popup_file_field_formatter':
    default:
      $popup_gallery = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array(
            'magnific-popup-iframe-gallery',
          ),
        ),
      );
      $has_items = FALSE;
      foreach ($items as $delta => $item) {
        // @todo Detect the scheme and do appropriate things (e.g. youtube:).
        if ($wrapper = file_stream_wrapper_get_instance_by_uri($item['uri'])) {
          $image = array(
            '#theme' => 'figure',
            // 'path' => $wrapper->getLocalThumbnailPath(),
            '#path' => image_style_url('magnific_popup_yt_preview_wplay', $wrapper->getLocalThumbnailPath()),
            '#alt' => $item['filename'],
            '#title' => $item['filename'],
            // Let the images be fluid.
            '#attributes' => array(
              'style' => array(
                'max-width: 100%;',
                'height: auto;',
              ),
            ),
          );
          $has_items = TRUE;
          $popup_gallery['item-' . $delta] = array(
            '#theme' => 'link',
            '#text' => render($image),
            '#path' => $wrapper->getExternalUrl(),
            '#options' => array(
              'attributes' => array(
                'class' => array(
                  'magnific-popup-iframe-gallery-item',
                ),
              ),
              'html' => TRUE,
            ),
          );
          unset($wrapper);
        }
      }
      if ($has_items) {
        $element[0] = $popup_gallery;
      }
      break;
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
