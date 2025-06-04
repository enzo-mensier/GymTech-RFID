<?php
// Configuration de la base de données
$host = 'b5ngvhiao4vdzqoon1ab-mysql.services.clever-cloud.com';
$dbname = 'b5ngvhiao4vdzqoon1ab';
$username = 'ugkgmch4wimnslaw';
$password = 'U7Ux4sLXrAhnBNYinG3v';

// Connexion à la base de données
try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    http_response_code(500);
    echo 'Erreur de connexion à la base de données : ' . $e->getMessage();
    exit();
}

// Récupération de l'UID RFID depuis l'URL (ex: get_user_color.php?uid=7AA3AD15)
if (!isset($_GET['uid'])) {
    http_response_code(400);
    echo 'UID manquant';
    exit();
}

$uid = $_GET['uid'];

// Requête pour trouver l'utilisateur correspondant à l'UID
$sql = "SELECT prenom FROM utilisateurs WHERE rfid_uid = :uid";
$stmt = $pdo->prepare($sql);
$stmt->bindParam(':uid', $uid);
$stmt->execute();

if ($stmt->rowCount() > 0) {
    $user = $stmt->fetch(PDO::FETCH_ASSOC);
    $prenom = strtolower($user['prenom']);

    // Associer le prénom à une couleur
    switch ($prenom) {
        case 'reine':
            $couleur = 'vert';
            break;
        case 'mael':
            $couleur = 'bleu';
            break;
        case 'enzo':
            $couleur = 'jaune';
            break;
        default:
            $couleur = 'rouge'; // si prénom non reconnu
    }
} else {
    $couleur = 'rouge'; // UID inconnu
}

// Retourne simplement la couleur en texte brut
echo $couleur;
?>
