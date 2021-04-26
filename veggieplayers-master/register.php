<?php
/* Registration process, inserts user info into the database 
   and sends account confirmation email message
 */

// Set session variables to be used on profile.php page
$_SESSION['email'] = $_POST['email'];
$_SESSION['Fname'] = $_POST['firstname'];
$_SESSION['Lname'] = $_POST['lastname'];
$_SESSION['username'] = $_POST['username'];

// Escape all $_POST variables to protect against SQL injections
$first_name = $mysqli->escape_string($_POST['firstname']);
$last_name = $mysqli->escape_string($_POST['lastname']);
$user_name = $mysqli->escape_string($_POST['username']);
$email = $mysqli->escape_string($_POST['email']);
$password = $mysqli->escape_string(password_hash($_POST['password'], PASSWORD_BCRYPT));
      
// Check if user with that email already exists
$result = $mysqli->query("SELECT * FROM user_data WHERE email='$email'") or die($mysqli->error());

// We know user email exists if the rows returned are more than 0
if ( $result->num_rows > 0 ) {
    
    $_SESSION['message'] = 'User with this email already exists!';
    header("Location: error.php");
    /*
    echo '<script type="text/javascript">
    window.location="http://veggiebirds.web.engr.illinois.edu/error.php";
    </script>';
    */
}
else { // Email doesn't already exist in a database, proceed...

    $sql = "INSERT INTO `user_data` (`Fname`, `Lname`, `username`, `email`, `password`) VALUES ('$first_name','$last_name','$user_name', '$email','$password')";
    
    //mysql_error();
    // Add user to the database
    if ( $mysqli->query($sql) ){

        
        $_SESSION['logged_in'] = true; // So we know the user has logged in

        header("Location: discussion.php");
        /*
        echo '<script type="text/javascript">
        //window.location="http://veggiebirds.web.engr.illinois.edu/discussion.php";
        //window,location="http://localhost:63342/veggieplayers-master/discussion.php";
        </script>';
        exit();
        */
    }

    else {
        $_SESSION['message'] = 'Registration failed!';

        header('Location: error.php');
        /*
        echo '<script type="text/javascript">
        //window.location="http://veggiebirds.web.engr.illinois.edu/error.php";
        //window,location="http://localhost:63342/veggieplayers-master/error.php";
        </script>';
        */
    }
    

}