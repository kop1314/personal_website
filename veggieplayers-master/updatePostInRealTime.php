<?php

session_start();
require 'db.php';

$trend_query = "SELECT `postID`, SUM(`isLiked`) AS total FROM `vote` GROUP BY `postID`ORDER BY total DESC
LIMIT 1";
if($result = $mysqli -> query($trend_query)) {
    $data = $result -> fetch_assoc();
    echo $data["postID"];
} else {
    echo '-1';
}

?>
