<?php
session_start();
require 'db.php';
//set session variable
$email = $_SESSION['email'];
$postID = (int)$_POST['postId'];

$viewerID_query = "SELECT `userID` FROM `user_data` WHERE `email` = '$email'";
if($result = $mysqli -> query($viewerID_query)){
    $row = $result -> fetch_assoc();
    $viewer_uid = $row["userID"];
    //check if vote relation between `postID` and `viewerID` exist
    $exist_query = "SELECT * FROM `vote` WHERE `postID` = '$postID' AND `viewerID` = '$viewer_uid'";
    if($result_4 = $mysqli->query($exist_query)){
        //row exists
        if($result_4->num_rows > 0) {
            echo '-1';
            exit();
        } else {
            $posterID_query = "SELECT `userID` FROM `post` WHERE `postID` = '$postID'";
            //echo "success 1";
            if($result_2 = $mysqli ->query($posterID_query)){
                $row_2 = $result_2 -> fetch_assoc();
                $poster_uid =$row_2["userID"];


                $insert_query = "INSERT INTO `vote` (`postID`, `posterID`, `viewerID`) VALUES ('$postID', '$poster_uid', '$viewer_uid')";
                //echo "success 2";
                if($result_3 = $mysqli -> query($insert_query)){
                    //echo "success 3";
                } else {
                    $_SESSION['message'] = 'Failed to insert new Vote! ' .gettype($postID) .$mysqli->error;
                    header("Location: error.php");
                    exit();
                }
            } else {
                $_SESSION['message'] = 'Failed to get posterID! ';
                header("Location: error.php");
                exit();
            }
        }
    } else {
        $_SESSION['message'] = 'Failed to check if row exists! '.$mysqli->error;
        header("Location: error.php");
        exit();
    }
} else {
    $_SESSION['message'] = 'Failed to get viewerID! ' .$email. ''.$mysqli->error;
    header("Location: error.php");
    exit();
}


?>