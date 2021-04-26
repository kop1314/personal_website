<?php
session_start();
require 'db.php';
//set session variable
$email = $_SESSION['email'];
$postID = $_POST['postID'];


//$postID = $_POST['postID'];
$display_comments_query = "SELECT * FROM `main_reply` WHERE `postID` = '$postID' ORDER BY `timestamp` DESC";
if($result = $mysqli -> query($display_comments_query)) {
    while($row = $result->fetch_assoc()) {
        $senderID = $row["senderID"];
        $reply_group_id = $row["reply_group_id"];
        $userName = getUserName($senderID, $mysqli);
        $text = $row["text"];
        $postID = $row["postID"];
        //reply_group_container has 1 main_reply and 0<= #sub_reply <= infinity
        echo '<div class="reply_group_container" id="'.$reply_group_id.'">';
        echo '<div class="reply_grid main_reply_container" id="main_reply_container_'.$reply_group_id.'">';

        echo '<div class="reply_item1">';
        echo '<img src="css/image/earlybirds.png" alt="earlybird">';
        echo '</div>';

        echo '<div class="reply_item2">';
        echo '<h>'.$userName.'</h>';
        echo '</div>';

        echo '<div class="reply_item3">';
        echo '<a href="#main_reply_container_'.$reply_group_id.'" class="reply_btn" id="'.$postID.'_'.$senderID.'_'.$reply_group_id.'">Reply</a>';
        echo '</div>';

        echo '<div class="reply_item4">';
        echo '<p>'.$text.'</p>';
        echo '</div>';

        echo '</div>';
        //echo sub reply
        echoSubReply($reply_group_id, $postID, $mysqli);

        echo '</div>';
    }
}

function getUserName($userID, $mysqli) {
    $userName_query = "SELECT `username` FROM `user_data` WHERE `userID` = '$userID'";
    if($result = $mysqli -> query($userName_query)) {
        $data = $result -> fetch_assoc();
        return $data["username"];
    }
    return NULL;
}

function echoSubReply($reply_group_id, $postID, $mysqli) {
    $sub_reply_query = "SELECT * FROM `sub_reply` WHERE `reply_group_id` = '$reply_group_id' ORDER BY `timestamp` DESC";
    if($result = $mysqli -> query($sub_reply_query)) {
        $count = 0;
        while($row = $result -> fetch_assoc()) {
            $senderID = $row["senderID"];
            $userName = getUserName($senderID, $mysqli);
            $text = $row["text"];
            echo '<div class="reply_grid sub_reply_container" id="sub_reply_container_'.$reply_group_id.'_'.$count.'">';

            echo '<div class="reply_item1">';
            echo '<img src="css/image/earlybirds.png" alt="earlybird">';
            echo '</div>';

            echo '<div class="reply_item2">';
            echo '<h>'.$userName.'</h>';
            echo '</div>';

            echo '<div class="reply_item3">';
            echo '<a href="#sub_reply_container_'.$reply_group_id.'_'.$count.'" class="reply_btn" id="'.$postID.'_'.$senderID.'_'.$reply_group_id.'">Reply</a>';
            echo '</div>';

            echo '<div class="reply_item4">';
            echo '<p>'.$text.'</p>';
            echo '</div>';

            echo '</div>';
            $count ++;
        }
    }
}

?>
