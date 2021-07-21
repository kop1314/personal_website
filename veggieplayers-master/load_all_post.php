<?php
session_start();
require 'db.php';
//set session variable
$email = $_SESSION['email'];

if(isset($_POST['page']))
{
    $page = $_POST['page'];
}
else{
    $page = 1;
}

$row_per_page = 10;
$start_from = ($page - 1) * $row_per_page;

$load_all_post_query = "SELECT * FROM `post` ORDER BY `timestamp` DESC LIMIT $start_from, $row_per_page";
$count_all_post_query = "SELECT * FROM `post`";

if($result = $mysqli -> query($load_all_post_query)){
    if(mysqli_num_rows($result) > 0){
        echo '<table class="table all_post_table">';
        $tmp_newest_postID = $_SESSION['newest_postID'];
        while($row = $result -> fetch_assoc()) {
            $postName = $row['postname'];
            $dots = "...";
            $numOfViewers = getNumOfViewers($row["postID"], $mysqli);
            $numOfLike = getNumOfLike($row["postID"], $mysqli);
            $like_flag = isLikePost($row["postID"], $email, $mysqli);
            //echo $numOfViewers;
            echo '<tr>';
            //echo '<td>';

            //store the newest postID
            if($row['postID'] > $tmp_newest_postID){
                $tmp_newest_postID = $row['postID'];
            }
            if($row['postID'] > $_SESSION['newest_postID'] && $_SESSION['newest_postID'] > -1) {
                //new post
                echo '<div class="post_display_btn_block new_post_block" id="post_display_btn_block_'.$row["postID"].'">';
            } else {
                //old posts
                echo '<div class="post_display_btn_block" id="post_display_btn_block_'.$row["postID"].'">';
            }
            //fire video
            /*
            echo '<video autoplay muted loop style="display: none;">';
            echo '<source src= "video/in_fire.webm" type="video/webm">';
            echo '<source src= "video/in_fire.mp4" type="video/mp4">';
            echo "Your browser does not support the video tag.";
            echo '</video>';
            */

            echo '<button class="post_display_btn post_display_btn_normal" id="'.$row["postID"].'">';

            echo '<span class="postname_container">';
            echo (strlen($postName) > 20) ? substr($postName,0, 19).$dots : $postName;;
            echo '</span>';

            echo '<span class="arrow_container">';
            echo '<i class="arrow up" id="up_'.$row["postID"].'"></i>';
            echo '<i class="arrow down" id="down_'.$row["postID"].'" style="display:none"></i>';
            echo '</span>';

            echo '</button>';
            echo '</div>';

            echo '<div class="discussion_container" id ="discussion_container_'.$row["postID"].'" style="display:none">';
            echo '<div class="text_container">';
            echo '<p>';
            echo $row["text"];
            echo '</p>';
            echo '</div>';

            echo '<div class="discussion_bar">';
            echo '<span class="bar_span eye_container" id="'.$row["postID"].'">';
            echo '<span>';
            echo '<i class="fas fa-eye"></i>';
            echo '</span>';
            echo '<span id="eye_'.$row["postID"].'">'.$numOfViewers.'</span>';
            echo '</span>';

            echo '<span class="bar_span comment_alt_container">';
            echo '<i class="far fa-comment-alt"></i>';
            echo '</span>';

            if($like_flag == 1){
                echo '<span class="bar_span thumbs_up_container thumbs_up_container_toggle" id="'.$row["postID"].'">';
                echo '<span>';
                echo '<i class="far fa-thumbs-up"></i>';
                echo '</span>';
                echo '<span id="thumbs_up_'.$row["postID"].'">'.$numOfLike.'</span>';
                echo '</span>';
            } elseif($like_flag == 0){
                echo '<span class="bar_span thumbs_up_container original_thumbs_up" id="'.$row["postID"].'">';
                echo '<span>';
                echo '<i class="far fa-thumbs-up"></i>';
                echo '</span>';
                echo '<span id="thumbs_up_'.$row["postID"].'">'.$numOfLike.'</span>';
                echo '</span>';
            } else {
                echo '<span class="bar_span thumbs_up_container original_thumbs_up" id="'.$row["postID"].'">';
                echo '<span>';
                echo '<i class="far fa-thumbs-up"></i>';
                echo '</span>';
                echo '<span id="thumbs_up_'.$row["postID"].'">-1</span>';
                echo '</span>';
            }
            echo '</div>';

            echo '<div class="discussion_form_container">';

            echo '<div class="reply_container" id="reply_container_'.$row["postID"].'">';
            echo '</div>';

            echo '<div class="comment_ui" id="comment_ui_'.$row["postID"].'">';

            echo '<div class="comment_ui_item comment_ui_item1 cancel_comment_btn" id="'.$row["postID"].'">';
            echo '<div class="scroll_btn scroll_btn_1">';
            echo '<span></span>';
            echo '<span></span>';
            echo '<span></span>';
            echo '</div>';
            echo '</div>';

            echo '<div class="comment_ui_item comment_ui_item2">';
            echo '<textarea class="comment_container" id="comment_container_'.$row["postID"].'" placeholder="say something..."></textarea>';
            echo '</div>';

            echo '<div class="comment_ui_item comment_ui_item3 send_comment_btn" id="'.$row["postID"].'_-1_-1">';
            echo '<div class="scroll_btn scroll_btn_2">';
            echo '<span></span>';
            echo '<span></span>';
            echo '<span></span>';
            echo '</div>';
            echo '</div>';

            /*
            echo '<button class="send_comment_btn" id="'.$row["postID"].'_-1_-1">SEND</button>';
            echo '<textarea class="comment_container" id="comment_container_'.$row["postID"].'" placeholder="say something..."></textarea>';
            echo '<button class="cancel_comment_btn" id="'.$row["postID"].'">CANCEL</button>';
            */

            echo '</div>';
            echo '</div>';

            echo '</div>';
           // echo '</td>';
            echo '</tr>';
        }
        echo '</table>';

        //set session vairable
        $_SESSION['newest_postID'] = $tmp_newest_postID;

        $count_result = $mysqli->query($count_all_post_query);
        $total_row = $count_result->num_rows;
        $total_page = ceil($total_row/$row_per_page);

        echo '<div class="pagination_btn_container">';
        if($page >= 1)
        {
            if($page == 1){
                echo '<button class="pagination_btn invalid_btn pre" id="'.($page-1).'"><</button>';
            } else {
                echo '<button class="pagination_btn valid_btn pre" id="'.($page-1).'"><</button>';
            }
        }

        for($x = 1; $x <= $total_page; $x++)
        {
            if($page == $x) {
                echo '<button class="pagination_btn valid_btn link active" id="'.$x.'">'.$x.'</button>';
            } else {
                echo '<button class="pagination_btn valid_btn link" id="'.$x.'">'.$x.'</button>';
            }
        }

        if($page <= $total_page)
        {
            if($page == $total_page){
                echo '<button class="pagination_btn invalid_btn next" id="'.($page+1).'">></button>';
            } else {
                echo '<button class="pagination_btn valid_btn next" id="'.($page+1).'">></button>';
            }
        }
        echo '</div>';


    } else {
        //no post yet
        //wait for code
        echo '<div class="init-post-grid">';
        echo '<div class="init-post-container">';
        echo '<h1>Whoops, no post yet!</h1>';
        echo '</div>';
        echo '</div>';
    }
} else{
    $_SESSION['message'] = 'Failed to load all posts';
    header("Location: error.php");
    exit();
}

function getNumOfViewers($postID, $mysqli) {
    $count_viewer_query = "SELECT COUNT(*) as total FROM `vote` WHERE `vote`.`postID` = '$postID'";
    if($result = $mysqli -> query($count_viewer_query)){
        $data = $result -> fetch_assoc();

        return (int)$data["total"];
    }
    return -1;
}

function getNumOfLike($postID, $mysqli) {
    $count_like_query = "SELECT COUNT(*) as total FROM `vote` WHERE `postID` = '$postID' AND `isLiked` = 1";
    if ($result = $mysqli -> query($count_like_query)) {
        $data = $result -> fetch_assoc();
        return (int)$data["total"];
    }
    return -1;
}

function isLikePost($postID, $email, $mysqli) {
    $viewer_uid = getUserID($email, $mysqli);
    $like_post_query = "SELECT `isLiked` FROM `vote` WHERE `postID` = '$postID' AND `viewerID` = '$viewer_uid'";
    if($result = $mysqli -> query($like_post_query)) {
        $data = $result -> fetch_assoc();
        //check if vote relation exist
        if (isset($data["isLiked"]))
        {
            return $data["isLiked"];
        }
        return 0;
    }
    return -1;

}

function getUserID($email, $mysqli) {
    $userID_query = "SELECT `userID` FROM `user_data` WHERE `email` = '$email'";
    if($result = $mysqli -> query($userID_query)) {
        $data = $result -> fetch_assoc();
        return $data["userID"];
    }
    return NULL;
}


?>
