<?php
session_start();
require 'db.php';
//set session variable
$email = $_SESSION['email'];

$postID = $_POST['postId'];
//1 for like 0 for dislike
$isLike = $_POST['isLike'];

$viewerID_query = "SELECT `userID` FROM `user_data` WHERE `email` = '$email'";
if($result = $mysqli -> query($viewerID_query)) {
    $row = $result -> fetch_assoc();
    $viewer_uid = $row["userID"];
    $setLike_query = "UPDATE `vote` SET `isLiked` = '$isLike' WHERE `postID` = '$postID' AND `viewerID` = '$viewer_uid'";
    if($result_2 = $mysqli -> query($setLike_query)) {

    } else {
        $_SESSION['message'] = 'Failed to like a post! ' .$email. ''.$mysqli->error;
        header("Location: error.php");
        exit();
    }

} else {
    $_SESSION['message'] = 'Failed to get viewerID! ' .$email. ''.$mysqli->error;
    header("Location: error.php");
    exit();
}

?>
