<?php
session_start();
require 'db.php';
//set session variable
$email = $_SESSION['email'];;
$postID = $_POST['postId'];

$delete_query = "DELETE FROM `post` WHERE `postID` = $postID";
if($result = $mysqli -> query($delete_query)) {
    echo "Delete: success";
}
else {
    $_SESSION['message'] = 'Failed to delete a post: '.$postID;
    header("Location: error.php");
    exit();
}


?>