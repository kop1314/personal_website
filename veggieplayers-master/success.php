<?php
/* Displays all error messages */
session_start();
?>
<!DOCTYPE html>
<html>
<head>
    <title>Success</title>
    <?php include 'css/css.html'; ?>
</head>
<body>
<div class="form">
    <h1>SUCCESS</h1>
    <p>
    <?php 
    if( isset($_SESSION['message']) AND !empty($_SESSION['message']) ): 
        echo $_SESSION['message'];    
    else:
        header("Location: error.php");
        /*echo '<script type="text/javascript">
        window.location="http://veggiebirds.web.engr.illinois.edu/error.php";
        </script>';
        */
    endif;
    ?>
    </p>     
    <a href="discussion.php"><button class="button button-block"/>Return</button></a>
</div>
</body>
</html>