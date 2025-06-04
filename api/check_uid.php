<?php
header('Content-Type: application/json');
require 'config.php';

$uid = filter_input(INPUT_GET, 'uid', FILTER_SANITIZE_STRING);

if (!$uid) {
    echo json_encode(["genre" => "inconnu"]);
    exit;
}

$sql = "SELECT genre FROM utilisateurs WHERE rfid_uid = ?";
$stmt = $pdo->prepare($sql);
$stmt->execute([$uid]);
$result = $stmt->fetch();

if ($result) {
    echo json_encode(["genre" => $result['genre']]);
} else {
    echo json_encode(["genre" => "inconnu"]);
}
?>
