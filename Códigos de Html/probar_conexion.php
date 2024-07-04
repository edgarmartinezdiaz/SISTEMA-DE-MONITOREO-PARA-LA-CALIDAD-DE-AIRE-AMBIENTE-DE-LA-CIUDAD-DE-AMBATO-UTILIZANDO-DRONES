<?php
// Archivo: obtener_datos.php

// Configuración de conexión a la base de datos
$servername = "localhost";
$username = "phpmyadmin";
$password = "tesis2024";
$dbname = "dron";

// Crear conexión
$conn = new mysqli($servername, $username, $password, $dbname);

// Verificar la conexión
if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

// Consulta SQL para obtener los datos más recientes de los sensores
$sql = "SELECT mq8, mg811, mq135, mq7, sds25, sds10 FROM sensores ORDER BY id DESC LIMIT 1";
$result = $conn->query($sql);

// Preparar los datos en formato JSON
$data = [];
if ($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    // Crear un array asociativo con los nombres de los sensores y sus valores
    $data = [
        'mq8' => $row['mq8'],
        'mg811' => $row['mg811'],
        'mq135' => $row['mq135'],
        'mq7' => $row['mq7'],
        'sds25' => $row['sds25'],
        'sds10' => $row['sds10']
    ];
} else {
    $data = [
        'error' => 'No se encontraron datos'
    ];
}

// Devolver los datos como JSON
header('Content-Type: application/json');
echo json_encode($data);

// Cerrar conexión
$conn->close();
?>
