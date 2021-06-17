<!DOCTYPE html>
<html>

<head>
    <meta charset="utf-8" />
    <title></title>

</head>
<body>
    <div class="video-box">
        abc
        <?php
            echo '<video width="600" height="300" muted loop style="display: none;">';
            echo '<source src= "video/in_fire.mp4" type="video/mp4">';
            echo "Your browser does not support the video tag.";
            echo '</video>';
        ?>

        <button id="btn-1">Show</button>
        <button id="btn-2">Hide</button>
    </div>
</body>
<script data-require="jquery" data-semver="2.1.1" src="//cdnjs.cloudflare.com/ajax/libs/jquery/2.1.1/jquery.min.js"></script>


<script>
    $(document).ready(function(){
        $(".video-box").on('click', '#btn-1', function(){
            console.log("show");
            $("video").show();
            $("video")[0].play();
        });
        $(".video-box").on('click', '#btn-2', function(){
            console.log("hide");
            $("video")[0].pause();
            $("video").hide();
        });
    });
</script>
</html>

