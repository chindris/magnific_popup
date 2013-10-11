(function ($) {
  Drupal.behaviors.magnific_popup_iframe = {
		attach: function (context, settings) {
      $('.magnific-popup-iframe-gallery').once('magnific_popup_iframe').magnificPopup({
        delegate: 'a',
        gallery: { enabled: true },
        disableOn: 300,
        type: 'iframe',
        mainClass: 'mfp-fade',
        removalDelay: 160,
        preloader: false,
        fixedContentPos: 'auto',
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
      });
    }
  };
})(jQuery);
