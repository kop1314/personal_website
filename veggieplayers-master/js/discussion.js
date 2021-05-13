var newest_postID = -1;

$(document).ready(function() {

    $('.post-form').hide();
    load_all_posts();
    load_my_posts();
    updatePostInRealTime();

    openPostHandler();
    likePostHandler();
    replyBtnHandler();
    sendCommentHandler();
    cancelCommentHandler();

    postPaginationBtnHandler();

    //create post handlers
    initPostBtnHandler();
    sendPostFormHandler();
    returnPostFormHandler();

    //user controls create, edit, delete post
    openPostFormHandler();
    editPostBtnHandler();
    deletePostBtnHandler();

    //edit form handlers
    editFormReturnHandler();
    editFormUpdateHandler();
    myPostpaginationBtnHandler();

    //Event: display post
    function openPostHandler(){
        $('.all_post_form').on('click', '.post_display_btn', function(e){

            var myid = this.id;
            if($('#up_'+myid).is(':visible')){
                $('.discussion_container').hide();
                $('.down').hide();
                $('.up').show();
                $('#up_'+myid).hide();
                $('#down_'+myid).show();
                $('#discussion_container_'+ myid).show();

                //load reply msg
                //remove old comments
                $('.reply_group_container').remove();
                //display old comments with new comments
                displayComments(myid);
            } else{
                $('#down_'+myid).hide();
                $('#up_'+myid).show();
                $('#discussion_container_'+myid).hide();
            }
            //create Vote object in db
            //since it is viewed by someone
            $.ajax({

                type: "POST",
                url: "create_vote.php",
                data: {"postId": myid},
                success:function(res)
                {
                    //alert("finish creating vote");
                    if(res=="-1"){
                        //rows exist in table
                        //SKIP
                    } else {
                        //first view
                        //add 1 to number of viewers
                        var text = $('#eye_' + myid).text();
                        //alert(text);
                        var num = parseInt(text, 10);
                        //alert(num);
                        var plus_1 = num + 1;
                        $('#eye_' + myid).html(plus_1.toString());
                    }
                }
            });

            e.preventDefault();

        });
    }

    //Event: like a post
    function likePostHandler(){
        $('.all_post_form').on('click', '.thumbs_up_container', function(e){
            //alert("thumb is clicked");
            var myid = this.id;
            $(this).toggleClass('original_thumbs_up thumbs_up_container_toggle');
            var isToggle = $(this).hasClass("thumbs_up_container_toggle");
            var postId = this.id;
            if (isToggle)
            {
                //like a post
                $.ajax({
                    type: "POST",
                    url: "setLike.php",
                    data: {"postId": postId, "isLike": 1},
                    success:function(res)
                    {
                        var text = $('#thumbs_up_' + myid).text();
                        var num = parseInt(text, 10);
                        var plus_1 = num + 1;
                        $('#thumbs_up_' + myid).html(plus_1.toString());
                        //alert("success");
                    }
                })
            } else{
                //dislike a post

                $.ajax({
                    type: "POST",
                    url: "setLike.php",
                    data: {"postId": postId, "isLike": 0},
                    success:function(res)
                    {
                        var text = $('#thumbs_up_' + myid).text();
                        var num = parseInt(text, 10);
                        var plus_1 = num - 1;
                        $('#thumbs_up_' + myid).html(plus_1.toString());
                        //alert("success");
                    }
                })
            }
            e.preventDefault();
        });
    }

    //Event: click reply
    function replyBtnHandler(){
        $('.all_post_form').on('click', '.reply_btn', function(e){
            //myID has the form: [postID]_[reply_receiver_id]_[reply_group_id]
            var myID = this.id;
            var id_array = myID.split("_");
            var postID = id_array[0];
            //var reply_group_id = id_array[1];
            var href = $(this).attr("href");
            var reply_target_name = $(href).find('h').text();
            $('#comment_container_' + postID).attr('placeholder', 'Reply To ' + reply_target_name);

            $('#comment_ui_' + postID).find('.send_comment_btn').attr('id', myID);

        });
    }

    //Event: send comment
    function sendCommentHandler(){
        $('.all_post_form').on('click', '.send_comment_btn', function(e){

            //send comment
            //myID has the form: [postID]_[reply_receiver_id]_[reply_group_id]
            var myID = this.id;
            var id_array = myID.split("_");
            var postID = id_array[0];
            var reply_receiver_id = id_array[1];
            var reply_group_id = id_array[2];
            var text = $('#comment_container_' + postID).val();
            if(text==""){
                return;
            }
            //0 for main reply, 1 for sub reply
            var sub_reply_flag = 1;


            //check if it is main reply
            if($('#comment_container_'+ postID).attr('placeholder') == "say something...")
            {
                sub_reply_flag = 0;
            }

            //alert(text);
            $.ajax({
                type: "POST",
                url: "sendComment.php",
                data: {"postID": postID, "text": text, "sub_reply_flag": sub_reply_flag,
                    "reply_group_id": reply_group_id, "reply_receiver_id": reply_receiver_id},
                success:function(res)
                {
                }
            });

            //remove old comments
            $('.reply_group_container').remove();

            //display all comments
            displayComments(postID);
            $('#comment_container_' + postID).val('');
            e.preventDefault();
        });
    }


    //Event cancel comment
    function cancelCommentHandler(){
        $('.all_post_form').on('click', '.cancel_comment_btn', function(e){
            var postID = this.id;
            //restore placeholder to default
            $('#comment_container_' + postID).attr("placeholder", "say something...");

            //empty comment container
            $("#comment_container_" + postID).val('');

            //restore send button id to default
            //myID has the form: [postID]_[reply_receiver_id]_[reply_group_id]
            //-1 for default reply_receiver_id
            //-1 for default reply_group_id
            var default_id = postID + "_-1_-1";
            $('#comment_ui_' + postID).find(".send_comment_btn").attr("id", default_id);
        });
    }

    //pagination
    //load all posts
    function load_all_posts(page)
    {
        $.ajax({
            type: "POST",
            url: "load_all_post.php",
            data: {"page": page},
            success:function(res)
            {
                //console.log("print result");
                $(".all_post_form").html(res);
                //console.log("finish");
            },
            error: function (jqXHR, exception) {
                var msg = '';
                if (jqXHR.status === 0) {
                    msg = 'Not connect.\n Verify Network.';
                } else if (jqXHR.status == 404) {
                    msg = 'Requested page not found. [404]';
                } else if (jqXHR.status == 500) {
                    msg = 'Internal Server Error [500].';
                } else if (exception === 'parsererror') {
                    msg = 'Requested JSON parse failed.';
                } else if (exception === 'timeout') {
                    msg = 'Time out error.';
                } else if (exception === 'abort') {
                    msg = 'Ajax request aborted.';
                } else {
                    msg = 'Uncaught Error.\n' + jqXHR.responseText;
                }
                $('.all_post_form').html(msg);
            },
            complete: function(res) {
                //setTimeout(delayUpdateAllPost, 5000);

            }
        })
    }

    //pagination load
    function postPaginationBtnHandler(){
        $('.all_post_form').on('click', '.valid_btn', function(e){
            var page = $(this).attr("id");
            load_all_posts(page);
            e.preventDefault();
        });
    }

    //first post button
    function initPostBtnHandler(){
        $('.mypostform').on('click', '.init-post-btn', function(e){
            $('.mypostform').hide();
            $('#postdiss').show();
            e.preventDefault();
        });
    }

    function sendPostFormHandler(){
        $('.post_mypost_post').click(function(e){
            var postname = $('input[name="post_postname"]').val();
            var text = $('textarea[name="post_text"]').val();
            $.ajax({
                type:"POST",
                url:"postdiss.php",
                data: {"postname": postname, "text": text},
                success:function(res){
                    //reload my posts;
                    load_all_posts();
                    load_my_posts();
                }
            });
            //clear input field
            $('input[name="post_postname"]').val("");
            $('textarea[name="post_text"]').val("");
            $('#postdiss').hide();
            $('.mypostform').show();
            e.preventDefault();
        });
    }

    function returnPostFormHandler(){
        //return event for post form
        $('.return_mypost_post').click(function(e)
        {
            //clear input field
            $('input[name="post_postname"]').val("");
            $('textarea[name="post_text"]').val("");
            $('#postdiss').hide();
            $('.mypostform').show();
            e.preventDefault();
        });
    }

    //post event for post form
    function openPostFormHandler(){
        $('.mypostform').on('click', '.post-btn', function(e){
            $('.mypostform').hide();
            $('#postdiss').show();
            e.preventDefault();
        });
    }

    //edit post
    function editPostBtnHandler(){
        $('.mypostform').on('click', '.edit_post', function(e)
        {
            //alert("eidt is clicked");
            e.preventDefault();
            $('.mypostform').hide();

            var postId = this.id;
            $.ajax({
                type: "POST",
                url:"load_single_post.php",
                data:{"postId": postId},
                dataType:"html",
                success:function(response){
                    $('#editpost').html(response);
                }
            });

            $('#editpost').show();
        });

    }

    //delete post
    function deletePostBtnHandler(){
        $('.mypostform').on('click', '.delete_post', function(e){
            var postId = this.id;
            //alert(postId);
            $.ajax({
                type: "POST",
                url:"deletepost.php",
                data:{"postId": postId},
                dataType: "html",
                success:function(response){
                    //reload my posts
                    load_all_posts();
                    load_my_posts();
                }
            });
            e.preventDefault();
        });
    }

    //bind event handlers for edit post event
    //return event for edit form
    function editFormReturnHandler(){
        $('#editpost').on('click', '.return_mypost_edit', function(e)
        {
            //alert("return is clicked");
            $('#editpost').hide();
            $('#edit_form').remove();
            $('.mypostform').show();
            e.preventDefault();
        });
    }

    //update event for edit form
    function editFormUpdateHandler(){
        $('#editpost').on('click', '.update_mypost_edit', function(e)
        {
            //alert("update is clicked");
            var postId = $('.update_mypost_edit').attr("id");
            var postname = $('input[name="edit_postname"]').val();
            var text = $('textarea[name="edit_text"]').val();
            //alert(postId +", " +postname + ", " + text);

            $.ajax({
                type:"POST",
                url:"editPost.php",
                data:{"postId":postId, "postname":postname, "text":text},
                success:function(response){
                    //reload my posts;
                    load_all_posts();
                    load_my_posts();
                }
            });
            $('#editpost').hide();
            $('#edit_form').remove();
            $('.mypostform').show();
            e.preventDefault();
        });
    }

    //pagination
    //load my posts
    function load_my_posts(page)
    {
        $.ajax({
            type: "POST",
            url: "load_all_my_post.php",
            data: {"page": page},
            success:function(res)
            {
                $('.mypostform').html(res);
            },
            error: function (jqXHR, exception) {
                var msg = '';
                if (jqXHR.status === 0) {
                    msg = 'Not connect.\n Verify Network.';
                } else if (jqXHR.status == 404) {
                    msg = 'Requested page not found. [404]';
                } else if (jqXHR.status == 500) {
                    msg = 'Internal Server Error [500].';
                } else if (exception === 'parsererror') {
                    msg = 'Requested JSON parse failed.';
                } else if (exception === 'timeout') {
                    msg = 'Time out error.';
                } else if (exception === 'abort') {
                    msg = 'Ajax request aborted.';
                } else {
                    msg = 'Uncaught Error.\n' + jqXHR.responseText;
                }
                $('.mypostform').html(msg);
            },
        });
    }

    //pagination button event handler
    function myPostpaginationBtnHandler(){
        $('.mypostform').on('click', '.valid_btn', function(e){
            var page = $(this).attr("id");
            load_my_posts(page);
            e.preventDefault();
        });
    }


    function displayComments(postID) {
        $.ajax({
            type: "POST",
            url: "displayComments.php",
            data: {"postID": postID},
            success:function(res)
            {
                $('#reply_container_' + postID).html(res);
            }
        });
    }

    function noPostIsVisible() {
        var flag = true;
        $(".discussion_container").each(function() {
            if($(this).is(':visible')) {
                flag = false;
            }
        });
        return flag;
    }

    function updateAllPostHandler() {
        if($('.pagination_btn.active').attr("id") == "1" && noPostIsVisible() == true){
            load_all_posts();
            updateTrendPost();
        }
        setTimeout(updateAllPostHandler, 5000);
    }

    function updateTrendPost(){

        $.ajax({
            type: "POST",
            url: "updatePostInRealTime.php",
            success:function(res)
            {
                //alert("hi");
                if(res == '-1'){
                    //alert("Failed to get data");
                }else if(res == '0'){
                    //no post is liked
                    //remove all videos
                    $(".post_display_btn").removeClass("post_display_btn_trend");
                    $(".post_display_btn").addClass("post_display_btn_normal");
                    var video = $("#video");
                    video.trigger('pause');
                    video.src='';
                    video.remove();
                }else{
                    //set all post_display_btn to normal
                    //delete all background videos
                    $(".post_display_btn").removeClass("post_display_btn_trend");
                    $(".post_display_btn").addClass("post_display_btn_normal");
                    var video = $("#video");
                    video.trigger('pause');
                    video.src='';
                    video.remove();

                    //load_all_posts();

                    var postID = res;
                    //alert(postID);
                    var btnID = "#post_display_btn_block_" + postID;
                    var video = $('<video />', {
                        id: 'video',
                        src: 'video/in_fire.mp4',
                        type: 'video/mp4',
                        loop: true,
                        muted:true,
                        autoplay: true
                    });
                    $(btnID).prepend(video);
                    $(btnID).find(".post_display_btn").removeClass("post_display_btn_normal");
                    $(btnID).find(".post_display_btn").addClass("post_display_btn_trend");
                }
            },
            complete:function(data)
            {
                //setTimeout(updateTrendPost, 5000);
            }
        });

    }

    /*
    function updateMostLikePost(){
        $ajax({
            type:"POST",
            url: "updateMostLikePostInRealTime.php",
            success:function(data){

            },
            complete:function (data){
                setTimeout(updateMostLikePost, 5000);
            }
        });
    }
    */


    function updatePostInRealTime() {
        setTimeout(updateAllPostHandler, 5000);
    }

});


