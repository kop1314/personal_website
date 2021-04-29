$(document).ready(function(){
    //The following function is from W3school
    //https://www.w3schools.com/howto/tryit.asp?filename=tryhow_css_smooth_scroll_jquery
    //access data: 03/20/21

    $("a").on('click', function(e) {
        // Make sure this.hash has a value before overriding default behavior
        if (this.hash !== "") {
            // Prevent default anchor click behavior
            e.preventDefault();

            // Store hash
            var hash = this.hash;

            // Using jQuery's animate() method to add smooth page scroll
            // The optional number (800) specifies the number of milliseconds it takes to scroll to the specified area
            $('html, body').animate({
                scrollTop: $(hash).offset().top
            }, 400, function(){
                // Add hash (#) to URL when done scrolling (default click behavior)
                window.location.hash = hash;
            });
        } // End if
    });


    $(window).scroll(function(){
        highlighNavItem();
        startAnimationOnScroll();
    });

    /*project section*/
    /*slick slider*/
    $('.card-slider').slick({
        slidesToShow: 3,
        slidesToScroll: 1,
        dots: true,
        arrows: true,
        autoplay: true,
        autoplaySpeed: 1000,
        infinite: false,
        responsive: [
            {
                breakpoint: 1024,
                settings:{
                    slidesToShow: 2,
                    slidesToScroll: 1
                }
            },
            {
                breakpoint: 600,
                settings:{
                    slidesToShow: 1,
                    slidesToScroll: 1,
                    arrows:false
                }
            }
        ]
    });

    function highlighNavItem(){
        var scrollPos = $(window).scrollTop();
        $('.section').each(function() {
            var offSetTop = $(this).offset().top;
            //var sectionHeight = $(this).clientHeight;
            var offSetHeight = $(this).outerHeight();
            //check if scroll into section view
            //adjust with -55 for fixed navbar
            if((offSetTop - 55) <= scrollPos &&
                scrollPos < offSetTop + offSetHeight - 55){
                //remove active class
                $('nav > ul > li > a').each(function() {
                    $(this).removeClass("active");
                });
                //add active class
                var id = $(this).attr('id');
                $('a[href="#'+ id +'"]').addClass("active");
                //console.log(id);
            }
        });
    }

    function startAnimationOnScroll(){
        //add active class
        var skill_section = $(".skill-container");
        var viewportTop = $(window).scrollTop();
        var viewportBottom = viewportTop + $(window).height();
        var sectionTop = skill_section.offset().top;
        var sectionBottom = sectionTop + skill_section.height();

        var title = $('.skill-list-item > p');
        var list = $('.skill-list-item > ul');

        if(sectionTop < viewportBottom && sectionBottom > viewportTop) {
            if(title.hasClass("start-title")){
                return;
            }
            if(list.hasClass("start-list")){
                return;
            }
            title.addClass("start-title");
            list.addClass("start-list");
        } else{
            title.removeClass("start-title");
            list.removeClass("start-list");
        }
    }
});