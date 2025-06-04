<?php
$host = 'b5ngvhiao4vdzqoon1ab-mysql.services.clever-cloud.com';
$dbname = 'b5ngvhiao4vdzqoon1ab';
$username = 'ugkgmch4wimnslaw';
$password = 'U7Ux4sLXrAhnBNYinG3v';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$db;charset=utf8", $user, $pass);
} catch (PDOException $e) {
    echo json_encode(["error" => "Database connection failed"]);
    exit;
}
?>
