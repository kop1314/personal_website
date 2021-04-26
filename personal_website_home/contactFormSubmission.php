<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Submission</title>
    <link rel="stylesheet" type="text/css" href="css/contactFormSubmission.css"/>
</head>
<body>
<div class="text_container">
    <?php
    $flag = $_REQUEST['flag'];
    if($flag == "1"){
        //thank you
        $line = "Thank you for your submission!";
        echo '<p class="thank_you_text">'.$line.'</p>';
        echo '<div class="back_btn_box">';
        echo '<a href="index.html">BACK</a>';
        echo '</div>';
    } else {
        //something went wrong
        $line_1 = "Something went wrong!";
        $line_2 = "Please try again.";
        echo '<p>'.$line_1.'</p>';
        echo '<p>'.$line_2.'</p>';
        echo '<div class="back_btn_box">';
        echo '<a href="index.html#contact-container-1">BACK</a>';
        echo '</div>';
    }
    ?>
</div>
</body>
</html>