<?php
    /*$myObj = array("postname" => "New Post", "text" => "Hi!");            $myJSON = json_encode($myObj);
                echo $myJSON;
    */
    $postID = $_POST['postID'];
    $nc = count($postID);
    if ($nc > 1){
            
        
        foreach($postID as $aPostID){
            
            $postID = $mysqli->escape_string($_POST['postID']);
            $sql = "SELECT * FROM `post` WHERE `postID` ='$aPostID'";
            if($postresult = $mysqli -> query($sql)){
                //$myObj->postname = $postresult['postname'];
                //$myObj->text = $postresult['text'];
                
                //$myObj->postname = "New Post";
                //$myObj->text = "Hi!";
                $myObj = array("postname" => $postresult['postname'], "text" => $postresult['text']);
                 $myJSON = json_encode($myObj);
                echo $myJSON;
            }
                
        }
    }