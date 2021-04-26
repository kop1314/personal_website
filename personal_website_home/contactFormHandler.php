<?php
$myEmail = 'felix121512@yahoo.com';

$name = $_POST['name'];
$email = $_POST['email'];
$message = $_POST['message'];

if($name && $email && $message){
    $email_subject = "Contact Form Submission: $name";
    $email_msg = "\nYou have received a new message. Here are the details:\n 
    Name: $name \n 
    Email: \n 
    $email \n 
    Message: \n 
    $message";
    mail($myEmail, $email_subject, $email_msg);
    header("Location: contactFormSubmission.php?flag=1");
} else {
    header("Location: contactFormSubmission.php?flag=0");
}

?>