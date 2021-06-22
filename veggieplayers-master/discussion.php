<?php
/* Page to post discussions */
require 'db.php';
session_start();
$_SESSION['newest_postID'] = -1;
$email = $_SESSION['email'];
?>

<!DOCTYPE html>
<html lang="en">
<head>
  <title>Discussions</title>
    <link rel="stylesheet" type="text/css" href="css/normalize.css">
    <link rel="stylesheet" type="text/css" href="css/discussion.css">
    <link rel="stylesheet" type="text/css" href="css/mypost.css">
    <link rel="stylesheet" type="text/css" href="css/post.css">
    <link rel="stylesheet" type="text/css" href="css/time.css">
    <!-----<link rel="stylesheet" type="text/css" href="css/reset.css">---->

    <!--------- Bootstrap datatable
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <link rel="stylesheet" href="http://cdn.datatables.net/1.10.2/css/jquery.dataTables.min.css">
    --------->

    <!------using fontAwesome------->
    <script src="https://kit.fontawesome.com/c8e4d183c2.js" crossorigin="anonymous"></script>
    <!---<script src="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.9.0/js/all.min.js"></script>--->
    <!---------
    <script src='http://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js'></script>
    -------->

    <!--------- Bootstrap datatable
    <script type="text/javascript" src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
    <script type="text/javascript" src="https://cdn.datatables.net/1.10.24/js/jquery.dataTables.min.js"></script>
    -------->

    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    <script src="js/discussion.js"></script>
</head>


<?php
if ($_SERVER['REQUEST_METHOD'] == 'POST'){


    if(isset($_POST['postdiss'])) {
    	REQUIRE 'postdiss.php';

    }

}

?>

<body>
    <nav>
        <!----------------logo---------------->
        <a href="#" class="logo">
            <!---
         <span class="fa-layers fa-fw">
             <i class="fas fa-dove fa-2x" data-fa-transform="shrink-2 right-70 up-2" style="color:#33FFEC"></i>
             <span class="fa-layers-text fa-inverse" data-fa-transform="shrink-1 right-30" style="color:#33FF8D; font-family: Arial, Helvetica, sans-serif; font-size:20px; font-weight: 900;">
                    Veggie
                </span>
                <span class="fa-layers-text fa-inverse" data-fa-transform="shrink-1 right-75" style="color:#FFB533; font-family: Arial, Helvetica, sans-serif; font-size:20px; font-weight: 900;">
                    Birds
                </span>
         </span>
         ----->
            <img src="css/image/logo1.png" alt="logo" width="150" height="40">
        </a>

        <ul>
            <li><a href="#">Home</a></li>
            <li><a href="#">About</a></li>
            <li><a href="#">Contact</a></li>
            <li><a href="logout.php">Logout</a></li>
            <!---------dropdown menu---------->
        </ul>
    </nav>

<div class="blended_grid">
<div class="pageHeader">
</div>
  
<div class="pageLeftMenu">
</div>
  
<div class="pageContent">
      <!-- Tab links -->
    <div class="tab">
      <!--<button id = "d_section" class="tablinks" onclick="openSection(event, 'Game_Info')">Game Info</button>-->
      <button id = "d_section" class="tablinks" onclick="openSection(event, 'Posts')">Posts</button>
      <button id ="my-post-section" class="tablinks" onclick="openSection(event, 'MyPosts')">My Posts</button>
    </div>

    <div class="tabcontent_container">
        <div id="Posts" class="tabcontent">
            <!---Reserve for ranking board---->
            <!----
            <div class="rank-board-container">
                <div class="rank-board-row" id="rank-board-row-1">
                    <video autoplay muted loop>
                        <source src= "video/in_fire.webm" type="video/webm">
                        <source src= "video/in_fire.mp4" type="video/mp4">
                        Your browser does not support the video tag.
                    </video>
                </div>

                <div class="rank-board-row" id="rank-board-row-2">
                    <video autoplay muted loop>
                        <source src= "video/in_fire.webm" type="video/webm">
                        <source src= "video/in_fire.mp4" type="video/mp4">
                        Your browser does not support the video tag.
                    </video>
                </div>

                <div class="rank-board-row" id="rank-board-row-3">
                </div>
            </div>
            ---->
            <div class="all_post_form">
            </div>
        </div>

        <div id="MyPosts" class="tabcontent">
            <!------Load when the website is opened------->
            <div class="mypostform">

            </div>

            <div class ="post-form" id="postdiss">
                <h1>Post your thoughts!</h1>
                <form action="discussion.php" method="post">
                    <div class="field-wrap title-wrap">
                        <label>
                            Postname (Title)<span class="req">*</span>
                        </label>
                        <input type="text" required autocomplete="off" name="post_postname"/>
                    </div>

                    <div class="field-wrap content-wrap">
                        <label>
                            Content<span class="req">*</span>
                        </label>
                        <textarea required autocomplete="off" name="post_text"></textarea>
                    </div>
                    <button class="post_mypost_post">POST</button>
                    <button class="return_mypost_post">RETURN</button>
                </form>

            </div>

            <!-------Echo content when edit is clicked------->
            <div class ="post-form" id="editpost">

            </div>
        </div>
    </div>
</div>
  
<div class="pageRightMenu">
</div>
  
<div class="pageFooter">
    <footer class="footer">Copyright &copy; Veggiebirds</footer>
</div>
  
</div>
    <script>
    function openSection(evt, section) {
        // Declare all variables
        var i, tabcontent, tablinks;

        // Get all elements with class="tabcontent" and hide them
        tabcontent = document.getElementsByClassName("tabcontent");
        for (i = 0; i < tabcontent.length; i++) {
            tabcontent[i].style.display = "none";
        }

        // Get all elements with class="tablinks" and remove the class "active"
        tablinks = document.getElementsByClassName("tablinks");
        for (i = 0; i < tablinks.length; i++) {
            tablinks[i].className = tablinks[i].className.replace(" active", "");
        }

        // Show the current tab, and add an "active" class to the button that opened the tab
        document.getElementById(section).style.display = "block";
        evt.currentTarget.className += " active";
    }

    document.getElementById("d_section").click();

    /*select all checkboxes
    The idea of following function "selectAll" come from
    http://www.includehelp.com/code-snippets/javascript-select-unselect-check-unckecck-all-checkboxes.aspx*/
    function selectAll(){
    	var items = document.getElementsByName("postID[]")
    	var selectBTN = document.getElementById("select_all_btn")
    	if(selectBTN.checked==true)
        {
            for(var i=0; i<items.length; i++){
					if(items[i].type=='checkbox')
						items[i].checked=true;
		    }
        } else {
            for(var i=0; i<items.length; i++){
					if(items[i].type=='checkbox')
						items[i].checked=false;
		    }
        }
    }
    </script>
    
</body>
</body>
</html>
