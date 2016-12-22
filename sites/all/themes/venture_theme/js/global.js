jQuery( function($) {
	
	$(document).ready(function(){
		
		// Main menu superfish
		$('#main-menu > ul').superfish({
			delay: 200,
			animation: {opacity:'show', height:'show'},
			speed: 'fast',
			cssArrows: false,
			disableHI: true
		});

		// Mobile Menu
		$('#navigation-toggle').sidr({
			name: 'sidr-main',
			source: '#sidr-close, #site-navigation',
			side: 'left'
		});
		$(".sidr-class-toggle-sidr-close").click( function() {
			$.sidr('close', 'sidr-main');
			return false;
		});
		
		// Close the menu on window change
		$(window).resize(function() {
			$.sidr('close', 'sidr-main');
		});
		
		// Fixed Header
		//$("#header-wrap.fixed-header").sticky({topSpacing:0});

	}); // End doc ready

	$(window).load(function(){
		// Homepage FlexSlider

		$('#main-menu > ul > li > a.sf-with-ul').append('<i class="fa fa-angle-down"></i>');
		$('#main-menu > ul > li li > a.sf-with-ul').append('<i class="fa fa-angle-right"></i>');

		$('#homepage-slider').flexslider({
			animation: 'slide',
			slideshow: true,
			smoothHeight: true,
			controlNav: false,
			directionNav: true,
			prevText: '<span class="fa fa-angle-left"></span>',
			nextText: '<span class="fa fa-angle-right"></span>',
			controlsContainer: ".flexslider-container"
		});
		
	}); // End on window load
	
});