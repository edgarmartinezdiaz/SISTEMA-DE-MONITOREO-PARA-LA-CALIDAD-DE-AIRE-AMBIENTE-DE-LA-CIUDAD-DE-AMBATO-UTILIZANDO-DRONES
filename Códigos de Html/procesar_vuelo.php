<?php
// Datos de conexión a la base de datos
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

// Obtener datos del formulario
$nombre = $_POST['nombre'];
$piloto = $_POST['piloto'];
$ubicacion = $_POST['ubicacion'];
$coordenadas = $_POST['coordenadas'];

// Preparar y bindear
$stmt = $conn->prepare("INSERT INTO vuelos (nombre, piloto, ubicacion, coordenadas) VALUES (?, ?, ?, ?)");
$stmt->bind_param("ssss", $nombre, $piloto, $ubicacion, $coordenadas);






if ($stmt->execute()) {
    // Redireccionar a la página de visualización con los parámetros necesarios
    header("Location: visualizacion.html?nombreProyecto=$nombre&piloto=$piloto&ubicacion=$ubicacion&coordenadas=$coordenadas");
    exit();
} else {
    echo "Error: " . $stmt->error;
}
// Ejecutar la declaración
//if ($stmt->execute()) {
  //  echo "Nuevo registro creado exitosamente";
//} else {
//    echo "Error: " . $stmt->error;
//}

// Cerrar conexión
$stmt->close();
$conn->close();
?>
