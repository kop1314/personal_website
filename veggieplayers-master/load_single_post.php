<?php
session_start();
require 'db.php';
//set session variable
$postID = (int)$_POST['postId'];
$email = $_SESSION['email'];

$postSQL = "SELECT * FROM `post` WHERE `post`.`postID` = $postID";
if($result = $mysqli -> query($postSQL)){

    $row = $result -> fetch_assoc();
    echo '<form method="post" autocomplete="off" id="edit_form">';
    echo '<div class="field-wrap title-wrap">';
    echo '<label>Postname (Title)<span class="req">*</span></label>';
    echo '<input type="text" required autocomplete="off"  name="edit_postname" value="'.$row['postname'].'"/>';
    echo ' </div>';
    echo '<div class="field-wrap content-wrap">';
    echo '<label>Content<span class="req">*</span></label>';
    echo '<textarea required autocomplete="off" name="edit_text">'.$row['text'].'</textarea>';
    echo '</div>';
    echo '<button class="update_mypost_edit" name="updatepost" id="'.$row['postID'].'"/>Update</button>';
    echo '<button class="return_mypost_edit" />RETURN</button>';
    echo '</form>';
}
else{
    $_SESSION['message'] = 'Failed to load a single post! '.$postID;
    header("Location: error.php");
    exit();
}
?>
