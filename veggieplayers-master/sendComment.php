<?php
session_start();
require 'db.php';
//set session variable
$email = $_SESSION['email'];

$postID = $_POST['postID'];
$text = $_POST['text'];
$sub_reply_flag = $_POST['sub_reply_flag'];
$reply_group_id = $_POST['reply_group_id'];
$reply_receiver_id = $_POST['reply_receiver_id'];


$userID = getUserID($email, $mysqli);

if($sub_reply_flag) {
    //sub reply
    $send_comment_query = "INSERT INTO `sub_reply` (`reply_group_id`, `text`, `senderID`, `receiverID`) VALUES ('$reply_group_id', '$text', '$userID', '$reply_receiver_id')";
    if($result = $mysqli -> query($send_comment_query)) {
    } else {
        $_SESSION['message'] = "Failed to insert sub_reply " .$mysqli->error;
        header("Location: error.php");
        exit();
    }
} else {
    //main reply
    $send_comment_query = "INSERT INTO `main_reply` (`postID`, `senderID`, `text`) VALUES ('$postID', '$userID', '$text')";
    if($result = $mysqli -> query($send_comment_query)) {
    } else {
        $_SESSION['message'] = "Failed to insert main_reply";
        header("Location: error.php");
        exit();
    }
}

function getUserID($email, $mysqli) {
    $userID_query = "SELECT `userID` FROM `user_data` WHERE `email` = '$email'";
    if($result = $mysqli -> query($userID_query)) {
        $data = $result -> fetch_assoc();
        return $data["userID"];
    }
    return NULL;
}

function getUserName($email, $mysqli) {
    $userName_query = "SELECT `username` FROM `user_data` WHERE `email` = '$email'";
    if($result = $mysqli -> query($userName_query)) {
        $data = $result -> fetch_assoc();
        return $data["username"];
    }
    return NULL;
}
?>
