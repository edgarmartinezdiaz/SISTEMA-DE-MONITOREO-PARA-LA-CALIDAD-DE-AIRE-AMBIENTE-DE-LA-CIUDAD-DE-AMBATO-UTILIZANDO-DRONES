//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Test Slave (1 or 2) ESP32 Lora Ra-02
//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
//String dato = "";
String sen_mq8, sen_mg811, sen_mq135, sen_mq7, sen_sds25, sen_sds10;
int pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8, pos9;
//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 18
#define rst 14
#define dio0 26
//----------------------------------------
//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = "";
String recibido = "";
// Configura la cantidad de valores para el promedio móvil
const int cantidadValores = 30;  // Ajusta según la cantidad de valores que deseas promediar

// Arreglos para almacenar los últimos valores recibidos de cada sensor
float valoresSensor1[cantidadValores];
float valoresSensor2[cantidadValores];
float valoresSensor3[cantidadValores];
float valoresSensor4[cantidadValores];
float valoresSensor5[cantidadValores];
float valoresSensor6[cantidadValores];

// Índices para recorrer los arreglos
int indiceSensor1 = 0;
int indiceSensor2 = 0;
int indiceSensor3 = 0;
int indiceSensor4 = 0;
int indiceSensor5 = 0;
int indiceSensor6 = 0;

// Contadores de valores almacenados
int totalValoresSensor1 = 0;
int totalValoresSensor2 = 0;
int totalValoresSensor3 = 0;
int totalValoresSensor4 = 0;
int totalValoresSensor5 = 0;
int totalValoresSensor6 = 0;
// Variables globales para los promedios
float promedio1 = 0.0;
float promedio2 = 0.0;
float promedio3 = 0.0;
float promedio4 = 0.0;
float promedio5 = 0.0;
float promedio6 = 0.0;

//----------------------------------------
//---------------------------------------- LoRa data transmission configuration.
////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT AND SELECT ONE OF THE "LocalAddress" VARIABLES BELOW. //
////////////////////////////////////////////////////////////////////////////

//byte LocalAddress = 0x02;       //--> address of this device (Slave 1).
byte LocalAddress = 0x03;       //--> address of this device (Slave 2).

byte Destination_Master = 0x01; //--> destination to send to Master (ESP32).
//----------------------------------------
//---------------------------------------- Millis / Timer to update temperature and humidity values from DHT11 sensor.
unsigned long previousMillis_UpdateDHT11 = 0;
const long interval_UpdateDHT11 = 2000;
//----------------------------------------

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination_Master);        //--> add destination address
  LoRa.write(LocalAddress);       //--> add sender address
  LoRa.write(Outgoing.length());  //--> add payload length
  LoRa.print(Outgoing);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it
}
//________________________________________________________________________________

//________________________________________________________________________________ Subroutines for receiving data (LoRa Ra-02).
void onReceive(int packetSize) {
  if (packetSize == 0) return;  //--> if there's no packet, return

  //---------------------------------------- read packet header bytes:
  int recipient = LoRa.read();        //--> recipient address
  byte sender = LoRa.read();          //--> sender address
  byte incomingLength = LoRa.read();  //--> incoming msg length
  //----------------------------------------

  // Clears Incoming variable data.
  Incoming = "";

  //---------------------------------------- Get all incoming data.
  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }
  //----------------------------------------

  //---------------------------------------- Check length for error.
  if (incomingLength != Incoming.length()) {
    Serial.println();
    Serial.println("error: message length does not match length");
    return; //--> skip rest of function
  }
  //----------------------------------------

  //---------------------------------------- Checks whether the incoming data or message for this device.
  if (recipient != LocalAddress) {
    Serial.println();
    Serial.println("This message is not for me.");
    return; //--> skip rest of function
  }
  //----------------------------------------

  //---------------------------------------- if message is for this device, or broadcast, print details:
  Serial.println();
  Serial.println("Received from: 0x" + String(sender, HEX));
  //Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + Incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  //Serial.println("Snr: " + String(LoRa.packetSnr()));
  //----------------------------------------

  //  pos1 = Incoming.indexOf('/');
  //  pos2 = Incoming.indexOf('&');
  //
  //  estado_re = Incoming.substring(0, pos1);
  //  direccion_re = Incoming.substring(pos1 + 1, pos2);
  //  metros_re = Incoming.substring(pos2 + 1, Incoming.length());
  //
  //  Serial.print(F("ESTADO  = "));  Serial.println(estado_re);
  //  Serial.print(F("DIRECCION = "));  Serial.println(direccion_re);
  //  Serial.print(F("METROS = "));  Serial.println(metros_re);

  // Calls the Processing_incoming_data() subroutine.
  Processing_incoming_data();
}


//________________________________________________________________________________ Subroutines to process data from incoming messages, then send messages to the Master.
void Processing_incoming_data() {
  //---------------------------------------- Conditions for sending messages to Master.
  /////////////////////////////////////////////////////////////////////////////////////////
  // PLEASE UNCOMMENT THE LINE OF CODE BELOW IF THIS CODE OR THIS DEVICE IS FOR SLAVE 1. //
  /////////////////////////////////////////////////////////////////////////////////////////

  if (Incoming = true) {
    //digitalWrite(LED_1_Pin, !digitalRead(LED_1_Pin));
    //digitalWrite(LED_2_Pin, !digitalRead(LED_2_Pin));

    //LED_1_State = digitalRead(LED_1_Pin);
    //LED_2_State = digitalRead(LED_2_Pin);

    Message = "";
    Message = "SL2," + String(promedio1) + "," + String(promedio2) + "," + String(promedio3) + "," + String(promedio4) + "," + String(promedio5) + "," + String(promedio6);

    Serial.println();
    Serial.println("Send message to Master");
    Serial.print("Message: ");
    Serial.println(Message);

    sendMessage(Message, Destination_Master);
  }
  //----------------------------------------

  //---------------------------------------- Conditions for sending messages to Master.
  /////////////////////////////////////////////////////////////////////////////////////////
  // PLEASE UNCOMMENT THE LINE OF CODE BELOW IF THIS CODE OR THIS DEVICE IS FOR SLAVE 2. //
  /////////////////////////////////////////////////////////////////////////////////////////

  //  if (Incoming == "SDS2") {
  //    digitalWrite(LED_1_Pin, !digitalRead(LED_1_Pin));
  //    digitalWrite(LED_2_Pin, !digitalRead(LED_2_Pin));
  //
  //    LED_1_State = digitalRead(LED_1_Pin);
  //    LED_2_State = digitalRead(LED_2_Pin);
  //
  //    Message = "";
  //    Message = "SL2," + String(h) + "," + String(t) + "," + String(LED_1_State) + "," + String(LED_2_State);
  //
  //    Serial.println();
  //    Serial.println("Send message to Master");
  //    Serial.print("Message: ");
  //    Serial.println(Message);
  //
  //    sendMessage(Message, Destination_Master);
  //  }
  //----------------------------------------
}
//________________________________________________________________________________


void setup() {
  // Inicializar el puerto serial a una velocidad específica (en baudios)
  Serial.begin(115200);
  // Inicializa los arreglos con ceros
  for (int i = 0; i < cantidadValores; i++) {
    valoresSensor1[i] = 0.0;
    valoresSensor2[i] = 0.0;
    valoresSensor3[i] = 0.0;
    valoresSensor4[i] = 0.0;
    valoresSensor5[i] = 0.0;
    valoresSensor6[i] = 0.0;
  }

  // Mensaje inicial
  Serial.println("ESP32 listo para recibir datos por Serial");
  //---------------------------------------- Settings and start Lora Ra-02.
  LoRa.setPins(ss, rst, dio0);

  Serial.println();
  Serial.println("Start LoRa init...");
  if (!LoRa.begin(915E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");
  //----------------------------------------
}

void loop() {
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis / Timer to update the temperature and humidity values ​​from the DHT11 sensor every 2 seconds (see the variable interval_UpdateDHT11).
  unsigned long currentMillis_UpdateDHT11 = millis();
  if (currentMillis_UpdateDHT11 - previousMillis_UpdateDHT11 >= interval_UpdateDHT11) {
    previousMillis_UpdateDHT11 = currentMillis_UpdateDHT11;

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    //h = dht11.readHumidity();
    // Read temperature as Celsius (the default)
    //t = dht11.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    //if (isnan(h) || isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    //h = 0;
    // t = 0.0;
    //}

    String dato = "";
    // Verificar si hay datos disponibles para leer
    while (Serial.available()) {
      // Leer el dato recibido
      dato += (char) Serial.read();
      //if (dato == 0) return;
      delay(5);

      // Imprimir el dato recibido
      //Serial.print("Dato recibido: ");
      //Serial.println(dato);
    }

    pos1 = dato.indexOf('$');
    pos2 = dato.indexOf('%');
    pos3 = dato.indexOf('&');
    pos4 = dato.indexOf('/');
    pos5 = dato.indexOf('-');
    pos6 = dato.indexOf('(');
    if (pos1 != -1 && pos2 != -1 && pos3 != -1 && pos4 != -1 && pos5 != -1) {

      sen_mq8 = dato.substring(0, pos1);
      sen_mg811 = dato.substring(pos1 + 1, pos2);
      sen_mq135 = dato.substring(pos2 + 1, pos3);
      sen_mq7 = dato.substring(pos3 + 1, pos4);
      sen_sds25 = dato.substring(pos4 + 1, pos5);
      sen_sds10 = dato.substring(pos5 + 1,  dato.length());

      // Lee los valores recibidos desde el puerto serial (asumiendo que llegan en el orden de los sensores)
      float nuevoValor1 = sen_mq8.toFloat();
      float nuevoValor2 = sen_mg811.toFloat();
      float nuevoValor3 = sen_mq135.toFloat();
      float nuevoValor4 = sen_mq7.toFloat();
      float nuevoValor5 = sen_sds25.toFloat();
      float nuevoValor6 = sen_sds10.toFloat();

      // Agrega los nuevos valores a los arreglos en las posiciones actuales de los índices
      agregarValor(valoresSensor1, nuevoValor1, indiceSensor1, totalValoresSensor1);
      agregarValor(valoresSensor2, nuevoValor2, indiceSensor2, totalValoresSensor2);
      agregarValor(valoresSensor3, nuevoValor3, indiceSensor3, totalValoresSensor3);
      agregarValor(valoresSensor4, nuevoValor4, indiceSensor4, totalValoresSensor4);
      agregarValor(valoresSensor5, nuevoValor5, indiceSensor5, totalValoresSensor5);
      agregarValor(valoresSensor6, nuevoValor6, indiceSensor6, totalValoresSensor6);

      // Calcula los promedios de los valores actuales
      promedio1 = calcularPromedio(valoresSensor1, totalValoresSensor1);
      promedio2 = calcularPromedio(valoresSensor2, totalValoresSensor2);
      promedio3 = calcularPromedio(valoresSensor3, totalValoresSensor3);
      promedio4 = calcularPromedio(valoresSensor4, totalValoresSensor4);
      promedio5 = calcularPromedio(valoresSensor5, totalValoresSensor5);
      promedio6 = calcularPromedio(valoresSensor6, totalValoresSensor6);

      // Envía los promedios calculados a través del puerto serial
      Serial.print("Promedios: ");
      Serial.print(promedio1); Serial.print(", ");
      Serial.print(promedio2); Serial.print(", ");
      Serial.print(promedio3); Serial.print(", ");
      Serial.print(promedio4); Serial.print(", ");
      Serial.print(promedio5); Serial.print(", ");
      Serial.println(promedio6);



      Serial.print(("sensor sen_mq8   ="));  Serial.println(sen_mq8);
      Serial.print(("sensor sen_mg811 =")); Serial.println(sen_mg811);
      Serial.print(("sensor sen_mq135 =")); Serial.println(sen_mq135);
      Serial.print(("sensor sen_mq7   =")); Serial.println(sen_mq7);
      Serial.print(("sensor sen_sds25 ="));  Serial.println(sen_sds25);
      Serial.print(("sensor sen_sds10 =")); Serial.println(sen_sds10);

      // recibido = "SL2," + String(sen_mq8) + "," + String(sen_mg811) + "," + String(sen_mq135) + "," + String(sen_mq7) + "," + String(sen_sds25) + "," + String(sen_sds10);
      //Serial.print("Message: ");
      //Serial.println(recibido);
    }
  }

  //----------------------------------------
  // servo.write(0);
  //---------------------------------------- parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------

  // Aquí puedes agregar más lógica de tu programa

  ///  delay(300); // Pequeña pausa para no saturar el procesador
}

// Función para agregar un nuevo valor al arreglo y actualizar el índice y el contador
void agregarValor(float valores[], float nuevoValor, int &indice, int &totalValores) {
  valores[indice] = nuevoValor;
  indice = (indice + 1) % cantidadValores;
  if (totalValores < cantidadValores) {
    totalValores++;
  }
}

// Función para calcular el promedio de los valores actuales en el arreglo
float calcularPromedio(float valores[], int totalValores) {
  float sum = 0.0;
  for (int i = 0; i < totalValores; i++) {
    sum += valores[i];
  }
  return sum / totalValores;
}

