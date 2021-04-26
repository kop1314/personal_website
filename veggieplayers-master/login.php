<?php
/* User login process, checks if user exists and password is correct */
// Escape email to protect against SQL injections
$email = $mysqli->escape_string($_POST['email']);
$result = $mysqli->query("SELECT * FROM user_data WHERE email='$email'");

if ( $result->num_rows == 0 ){ // User doesn't exist
    $_SESSION['message'] = "User with that email doesn't exist!";
    header("Location: error.php");
    /*
    echo '<script type="text/javascript">
    window.location="http://veggiebirds.web.engr.illinois.edu/error.php";
    </script>';
    */
}
else { // User exists
    $user = $result->fetch_assoc();

    if ( password_verify($_POST['password'], $user['password']) ) {
        
        $_SESSION['email'] = $user['email'];
        $_SESSION['Fname'] = $user['Fname'];
        $_SESSION['Lname'] = $user['Lname'];
        $_SESSION['username'] = $user['username'];
        
        // This is how we'll know the user is logged in
        $_SESSION['logged_in'] = true;
        header("Location: discussion.php");
        /*
        echo '<script type="text/javascript">
        //window.location="http://veggiebirds.web.engr.illinois.edu/discussion.php";
        window,location="http://localhost:63342/veggieplayers-master/discussion.php";
        </script>';
        exit();
        */
    }
    else {
        $_SESSION['message'] = "You have entered wrong password, try again!";
        /*
        echo '<script type="text/javascript">
        window.location="http://veggiebirds.web.engr.illinois.edu/error.php";
        </script>';
        */
    }
}

