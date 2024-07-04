<?php
$servername = "localhost";
$username = "phpmyadmin";
$password = "tesis2024";
$dbname = "dron";

// Obtener el ID del proyecto de la URL
$proyecto_id = isset($_GET['proyecto_id']) ? intval($_GET['proyecto_id']) : 0;

if ($proyecto_id === 0) {
    echo json_encode([]);
    exit;
}

// Crear conexión
$conn = new mysqli($servername, $username, $password, $dbname);

// Verificar conexión
if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

// Consulta para obtener los detalles del proyecto
$sql = "SELECT nombre, piloto, ubicacion, coordenadas FROM vuelos WHERE id = $proyecto_id";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    $detallesProyecto = [
        'nombre' => $row['nombre'],
        'piloto' => $row['piloto'],
        'ubicacion' => $row['ubicacion'],
        'coordenadas' => $row['coordenadas']
    ];
    echo json_encode($detallesProyecto);
} else {
    echo json_encode([]);
}

// Cerrar la conexión
$conn->close();
?>
