<?php
header('Content-Type: application/json');

// Configuración de conexión a la base de datos
$servername = "localhost";
$username = "phpmyadmin";
$password = "tesis2024";
$dbname = "dron";

try {
    // Crear conexión PDO
    $pdo = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); // Manejo de excepciones
    
    // Determinar el caso según el parámetro 'q' enviado por GET
    switch ($_GET['q']) {
        // Buscar Último Dato
        case 1:
            $statement = $pdo->prepare("SELECT mq8, mg811, mq135, mq7, sds25, sds10 FROM sensores ORDER BY id DESC LIMIT 1");
            $statement->execute();
            $results = $statement->fetch(PDO::FETCH_ASSOC);
            echo json_encode($results);
            break;

        // Buscar Todos los datos
        default:
            $statement = $pdo->prepare("SELECT mq8, mg811, mq135, mq7, sds25, sds10 FROM sensores ORDER BY id ASC");
            $statement->execute();
            $results = $statement->fetchAll(PDO::FETCH_ASSOC);
            echo json_encode($results);
            break;
    }
} catch (PDOException $e) {
    // Capturar cualquier excepción PDO
    http_response_code(500);
    echo json_encode(['error' => ['message' => $e->getMessage()]]);
    exit;
}
?>
