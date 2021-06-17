<?php
/* Database connection settings */

$host = 'localhost';
$user = 'root';
$pass = '';
$db = 'veggiebirds_users';
$mysqli = new mysqli($host,$user,$pass,$db) or die($mysqli->error);

/*
$host = 'sql101.epizy.com';
$user = 'epiz_28620757';
$pass = 'kUtH7LVL9I';
$db = 'epiz_28620757_veggiebirds_users';
$mysqli = new mysqli($host,$user,$pass,$db) or die($mysqli->error);
*/
