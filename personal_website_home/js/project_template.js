$(document).ready(function(){
    //scroll to top when refresh page
    $(window).on('beforeunload', function(){
        $(window).scrollTop(0);
    });

    $(window).scroll(function(){
        highlightVerticalNavItem();
    });

    $('.toggle').click(function(){
        $('.toggle').toggleClass('active');
        $('#top-horizontal-navigation-bar').toggleClass('active');
        $('.project-info-grid').toggleClass('active');
    })

    function highlightVerticalNavItem(){
        var scrollPos = $(window).scrollTop();
        $('.project-info-section').each(function() {
            console.log($(this).attr('id'));
            var offSetTop = $(this).offset().top;
            //var sectionHeight = $(this).clientHeight;
            var offSetHeight = $(this).outerHeight();
            //check if scroll into section view
            //adjust with -55 for fixed navbar
            if((offSetTop - 100) <= scrollPos &&
                scrollPos < offSetTop + offSetHeight - 100){
                /*if((offSetTop - 55) <= scrollPos &&
                        scrollPos < offSetTop + offSetHeight - 55){*/
                //remove active class
                $('.content-left > nav > ul > li > a').each(function() {
                    $(this).removeClass("active");
                });
                //add active class
                var id = $(this).attr('id');
                $('a[href="#'+ id +'"]').addClass("active");
                //console.log(id);
            }
        });
    }


})