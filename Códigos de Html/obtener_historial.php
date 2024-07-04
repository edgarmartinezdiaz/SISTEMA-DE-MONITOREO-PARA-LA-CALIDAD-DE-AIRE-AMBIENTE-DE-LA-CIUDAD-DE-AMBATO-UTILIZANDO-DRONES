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

// Obtener el ID del proyecto de la URL
$proyecto_id = isset($_GET['proyecto_id']) ? intval($_GET['proyecto_id']) : 0;

if ($proyecto_id === 0) {
    echo json_encode([]);
    exit;
}

// Consulta para obtener el historial de datos del proyecto seleccionado
$sql = "SELECT fecha, mq8, mg811, mq135, mq7, sds25, sds10 
        FROM sensores 
        WHERE idVuelos = $proyecto_id 
        ORDER BY fecha DESC 
        LIMIT 100";
$result = $conn->query($sql);

$historial = [];

if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $historial[] = [
            'fecha' => $row['fecha'],
            'mq8' => $row['mq8'],
            'mg811' => $row['mg811'],
            'mq135' => $row['mq135'],
            'mq7' => $row['mq7'],
            'sds25' => $row['sds25'],
            'sds10' => $row['sds10']
        ];
    }
} else {
    echo json_encode([]);
    exit;
}

echo json_encode($historial);

// Cerrar la conexión
$conn->close();
?>
