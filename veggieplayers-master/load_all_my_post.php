<?php
/*
 * The idea of pagination with php comes from the following youtube video
 * https://www.youtube.com/watch?v=6Ou-FCoW--Q
 * access date: 03/31/2021
 */
session_start();
require 'db.php';
$email = $_SESSION['email'];

if(isset($_POST['page']))
{
    $page = $_POST['page'];
}
else{
    $page = 1;
}

$row_per_page = 3;
$start_from = ($page - 1) * $row_per_page;

//$sql = "SELECT * FROM `post` LIMIT $start_from, $row_per_page";
//$result = $mysqli->query($sql);
$limit_query = "SELECT * FROM `post`, `user_data` WHERE `user_data`.`userID` = `post`.`userID` AND `email` = '$email' ORDER BY `timestamp` DESC LIMIT $start_from, $row_per_page";
$count_sql = "SELECT * FROM `post` , `user_data` WHERE `user_data`.`userID` = `post`.`userID` AND `email` = '$email'";

if($result = $mysqli -> query($limit_query)){
    if(mysqli_num_rows($result) > 0){
        /*Have more than one post*/
        echo '<table class="table my_post_table" id="my-posts-table">';
        while($row = $result -> fetch_assoc()) {
            $numOfLike = getNumOfLike($row['postID'], $mysqli);
            $postName = $row['postname'];
            $dots = "...";
            echo '<tr id="'.$row['postID'].'">';
            //echo $row['postname'];
            echo '<td class="table_cell">';
            echo '<span class="title_container">';
            echo (strlen($postName) > 20) ? substr($postName,0, 19).$dots : $postName;
            echo '</span>';

            echo '<span class="tool_container">';
            echo '<span class="like_span self_like_container">';
            echo '<i class="fas fa-heart"></i>';
            echo '<span>'.$numOfLike.'</span>';
            echo '</span>';

            echo '<span class ="tool_span edit_post" id="'.$row['postID'].'">';
            echo '<i class="fas fa-edit"></i>';
            echo '</span>';

            echo '<span class ="tool_span delete_post" id="'.$row['postID'].'">';
            echo '<i class="fas fa-trash-alt"></i>';
            echo '</span>';
            echo '</span>';

            //echo '<button class ="edit_post" id="'.$row['postID'].'" name="edit_post">Edit</button>';
            //echo '<button class ="delete_post" id="'.$row['postID'].'" name= "delete_post">Delete</button>';
            echo '</td>';
            echo '</tr>';
        }
        echo '<tr>';
        echo '<td class="post_btn_cell">';
        echo '<button class = "create-btn post-btn" name="create_post" onclick="hideMyPosts()">+</button>';
        echo '</td>';
        echo '</tr>';
        echo '</table>';
        //$count_sql = "SELECT * FROM `post` WHERE `user_data`.`userID` = `post`.`userID` AND `email` = '$email'";

        $count_result = $mysqli->query($count_sql);
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

    }
    else{
        /*No post*/
        echo '<div class="init-post-grid">';
        echo '<div class="init-post-container">';
        echo '<h1>Whoops, no post yet!</h1>';
        echo '<button class="create-btn init-post-btn" name="create_post" onclick="hideMyPosts()">Create</button>';
        echo '</div>';
        echo '</div>';
    }
}
else{
    $_SESSION['message'] = 'Post Failed';
    //echo '<script type="text/javascript">window.location="http://veggiebirds.web.engr.illinois.edu/error.php";</script>';
    header("Location: error.php");
    exit();
}

function getNumOfLike($postID, $mysqli) {
    $count_like_query = "SELECT COUNT(*) as total FROM `vote` WHERE `postID` = '$postID' AND `isLiked` = 1";
    if ($result = $mysqli -> query($count_like_query)) {
        $data = $result -> fetch_assoc();
        return (int)$data["total"];
    }
    return -1;
}

?>
