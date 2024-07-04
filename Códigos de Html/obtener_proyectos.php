<?php
$servername = "localhost";
$username = "phpmyadmin";
$password = "tesis2024";
$dbname = "dron";

// Crear conexión
$conn = new mysqli($servername, $username, $password, $dbname);

// Verificar conexión
if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

// Consulta para obtener los proyectos y pilotos
$sql = "SELECT id, nombre, piloto FROM vuelos";
$result = $conn->query($sql);

$proyectos = [];

if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $proyectos[] = [
            'id' => $row['id'],
            'nombre' => $row['nombre'],
            'piloto' => $row['piloto']
        ];
    }
} else {
    echo json_encode([]);
    exit;
}

echo json_encode($proyectos);

// Cerrar la conexión
$conn->close();
?>
