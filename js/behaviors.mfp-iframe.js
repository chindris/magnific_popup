(function ($) {
  Drupal.behaviors.magnific_popup_iframe = {
		attach: function (context, settings) {
      var common_options = {
        disableOn: 300,
        mainClass: 'mfp-fade',
        removalDelay: 160,
        preloader: false,
        fixedContentPos: 'auto'
      };
      var gallery_options = {
        gallery: {enabled: true},
        type: 'iframe',
        delegate: 'a',
        iframe: {
          markup: '<div class="mfp-iframe-scaler"><div class="mfp-close"></div><iframe class="mfp-iframe" id="mfp-iframe" frameborder="0" seamless="seamless" allowfullscreen="allowfullscreen"></iframe></div>',
          patterns: {
            youtube: {
              index: 'youtube.com/',
              id: 'v=',
              src: '//www.youtube.com/embed/%id%?rel=0&modestbranding=1&playerapiid=mfp-iframe&controls=2&autoplay=1'
            }
          }
        }
      };
      var singleton_options = {
        type: 'image',
      };
      $.extend(singleton_options, common_options);
      $('.magnific-popup-no-gallery-item').once('magnific_popup').magnificPopup(singleton_options);
      $.extend(gallery_options, common_options);
      $('.magnific-popup-iframe-gallery').once('magnific_popup').each(function(el, i) {
        $(this).magnificPopup(gallery_options);
      });
    }
  };
})(jQuery);
