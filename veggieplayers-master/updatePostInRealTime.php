<?php

session_start();
require 'db.php';

$trend_query = "SELECT `postID`, SUM(`isLiked`) AS total FROM `vote` GROUP BY `postID` 
HAVING SUM(`isLiked`) > 0 ORDER BY total DESC LIMIT 1";
if($result = $mysqli -> query($trend_query)) {
    $data = $result -> fetch_assoc();
    if($result -> num_rows > 0){
        echo $data["postID"];
    } else {
        echo '0';
    }
} else {
    echo '-1';
}

?>
