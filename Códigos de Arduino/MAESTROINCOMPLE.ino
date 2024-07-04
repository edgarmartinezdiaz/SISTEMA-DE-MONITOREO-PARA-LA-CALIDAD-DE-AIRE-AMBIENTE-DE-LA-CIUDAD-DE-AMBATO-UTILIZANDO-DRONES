//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
//----------------------------------------
//---------------------------------------- OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//----------------------------------------

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 18
#define rst 14
#define dio0 26

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6


//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
// Replace the next variables with your SSID/Password combination
//const char* ssid = "oficina";
//const char* password = "oficina123";

const char* ssid = "Domotik";
const char* password = "SalomE...123";
// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
//const char* mqtt_server = "35.232.94.172";
const char* mqtt_server = "3.128.194.242";

WiFiClient espClient;
PubSubClient client(espClient);


//----------------------------------------
//======================================================================
//                MQTT  topicos
//======================================================================
int resultado;
const char* topic_estado = "servo/estado";          // Topic MQTT actuadores
const char* topic_direccion = "servo/direccion";          // Topic MQTT actuadores
const char* topic_metros = "servo/metros";          // Topic MQTT actuadores
const char* topic_idvuelos = "idvuelos";

char dato1[10];
int smq8 = 0;
    String mq8,mg811,mq135,mq7,sds25,sds10; 

uint8_t metros = 0;
uint8_t idvuel = 0;
uint8_t estado = false;
uint8_t direccion = false;
String salida;


//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = "";
String output;
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
byte LocalAddress = 0x01;               //--> address of this device (Master Address).
byte Destination_ESP32_Slave_1 = 0x02;  //--> destination to send to Slave 1 (ESP32).
byte Destination_ESP32_Slave_2 = 0x03;  //--> destination to send to Slave 2 (ESP32).
//----------------------------------------

//---------------------------------------- Variable declaration for Millis/Timer.
unsigned long previousMillis_SendMSG = 0;
const long interval_SendMSG = 1000;
//----------------------------------------

// Variable declaration to count slaves.
byte Slv = 0;
StaticJsonDocument<128> doc;

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination);        //--> add destination address
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
  //----------------------------------------
  // Process the Incoming message to extract values for SL1 and SL2
  String sl1Data = "";
  String sl2Data = "";

  if (Incoming.startsWith("SL1")) {
    sl1Data = Incoming.substring(4); // Extract the part after "SL1,"
  } else if (Incoming.startsWith("SL2")) {
    sl2Data = Incoming.substring(4); // Extract the part after "SL2,"
  }

  // Split the SL1 and SL2 data into individual components
  String sl1Components[4];
  String sl2Components[6];

  if (sl1Data.length() > 0) {
    int index = 0;
    int startIndex = 0;
    int commaIndex = sl1Data.indexOf(',');

    while (commaIndex != -1 && index < 4) {
      sl1Components[index++] = sl1Data.substring(startIndex, commaIndex);
      startIndex = commaIndex + 1;
      commaIndex = sl1Data.indexOf(',', startIndex);
    }
    if (index < 4) {
      sl1Components[index] = sl1Data.substring(startIndex);
    }
  }

  if (sl2Data.length() > 0) {
    int index = 0;
    int startIndex = 0;
    int commaIndex = sl2Data.indexOf(',');

    while (commaIndex != -1 && index < 6) {
      sl2Components[index++] = sl2Data.substring(startIndex, commaIndex);
      startIndex = commaIndex + 1;
      commaIndex = sl2Data.indexOf(',', startIndex);
    }
    if (index < 6) {
      sl2Components[index] = sl2Data.substring(startIndex);
    }
  }

  //---------------------------------------- if message is for this device, or broadcast, print details:
  Serial.println();
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Message: " + Incoming);

  if (sl1Data.length() > 0) {
    Serial.println("SL1 Data: Counter: " + sl1Components[0] + ", Button State: " + sl1Components[1] + ", LED 1 State: " + sl1Components[2] + ", LED 2 State: " + sl1Components[3]);
  }

  if (sl2Data.length() > 0) {
    Serial.println("SL2 Data: MQ-8: " + sl2Components[0] + ", MG-811: " + sl2Components[1] + ", MQ-135: " + sl2Components[2] + ",MQ-7: " + sl2Components[3] + ", SDS-25: " + sl2Components[4] + ", SDS-10: " + sl2Components[5]);

    // Simulación de datos
    mq8 = sl2Components[0];
    mg811 = sl2Components[1];
    mq135 = sl2Components[2];
    mq7 = sl2Components[3];
    sds25 = sl2Components[4];
    sds10 = sl2Components[5];

    //    client.publish("dron/MQ-8", mq8.c_str());
    //    client.publish("dron/MG-811", mg811.c_str());
    //    client.publish("dron/MQ-135", mq135.c_str());
    //    client.publish("dron/MQ7",  mq7.c_str());
    //    client.publish("dron/SDS25", sds25.c_str());
    //    client.publish("dron/SDS10", sds10.c_str());
    doc["DEVICE"] = "nodo1";
    doc["idvuel"] = idvuel ;
    doc["mq8"] = mq8 ;
    doc["mg811"] = mg811 ;
    doc["mq135"] = mq135;
    doc["mq7"] = mq7 ;
    doc["sds25"] = sds25 ;
    doc["sds10"] = sds10 ;

    serializeJson(doc, output);
    Serial.print("Publish message: ");
    Serial.println(output);
    client.publish("test", output.c_str());
  }

  //----------------------------------------
  //----------------------------------------

  //---------------------------------------- if message is for this device, or broadcast, print details:
  Serial.println();
  Serial.println("Received from: 0x" + String(sender, HEX));
  //Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + Incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  //Serial.println("Snr: " + String(LoRa.packetSnr()));
  //----------------------------------------
}
//________________________________________________________________________________

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA RECEIVER ");
  display.display();

  Serial.println("LoRa Receiver Test");


  //---------------------------------------- Settings and start Lora Ra-02.
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  Serial.println("Start LoRa init...");
  if (!LoRa.begin(BAND)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0, 10);
  display.println("LoRa Initializing OK!");
  display.display();
  //----------------------------------------
}
//________________________________________________________________________________
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  int resultado = messageTemp.toInt();

  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  /*if (String(topic) == "servo/estado") {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if (messageTemp == "off") {
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
    }*/

  if ( strcmp(topic, topic_metros) == 0 ) {
    Serial.println("T   E   M   P   E   R   A   T   U   R   A       B   A   J   A ");
    Serial.println("                           B    A   J   A:" + String(resultado));
    metros = resultado;

  }
  if ( strcmp(topic, topic_idvuelos) == 0 ) {
    Serial.println("VUEEEELOOOOOOOOOOSSS ");
    Serial.println("                           IIIIIIIIIII    DDDDDDD:" + String(resultado));
    idvuel = resultado;

  }

  //  ESTADO 1
  if ( strcmp(topic, topic_estado) == 0 ) {
    if ((char)message[0] == 'i') {
      Serial.println("SERVO ENCENDIDO"); estado = true;
      //flag_ca1_e = true;
    }
    if ((char)message[0] == 'j') {
      Serial.println("SERVO APAGADO");  estado = false;
      //flag_ca1_e = true;
    }
  }

  //  DIRECCION 1
  if ( strcmp(topic, topic_direccion) == 0 ) {
    if ((char)message[0] == 'a') {
      Serial.println("BAJANDO       BAJANDO       BAJANDO       BAJANDO       BAJANDO       "); direccion = true;
      //flag_ca1_e = true;
    }
    if ((char)message[0] == 'b') {
      Serial.println("SUBIENDO       SUBIENDO       SUBIENDO       SUBIENDO       SUBIENDO  "); direccion = false;
      //flag_ca1_e = true;
    }
  }

  salida = String(estado) + "/" + String(direccion) + "&" + String(metros);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(topic_metros);
      client.subscribe(topic_idvuelos);
      client.subscribe(topic_direccion);
      client.subscribe(topic_estado);
      //client.subscribe("servo / estado");
      //client.subscribe("servo / metros");
      //client.subscribe("servo / direccion");
    } else {
      Serial.print("failed, rc = ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}
//________________________________________________________________________________

//________________________________________________________________________________ VOID LOOP
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis or Timer to send message / command data to slaves every 1 second (see interval_SendCMD variable).
  // Messages are sent every one second is alternately.
  // > Master sends message to Slave 1, delay 1 second.
  // > Master sends message to Slave 2, delay 1 second.

  //////////////////////////////////////////
  //  StaticJsonDocument<128> doc;
  //
  //  int randomNumber = random(1, 100);
  //  doc["DEVICE"] = "nodo1";
  //  doc["idvuel"] = idvuel;
  //  doc["mq8"] = String(randomNumber);
  //  doc["mg811"] = String(randomNumber);
  //  doc["mq135"] = String(randomNumber);
  //  doc["mq7"] = String(randomNumber);
  //  doc["sds25"] = String(randomNumber);
  //  doc["sds10"] = String(randomNumber);
  //
  //
  //  String output;
  //
  //  serializeJson(doc, output);
  //
  //  Serial.print("Publish message: ");
  //  Serial.println(output);
  //  client.publish("test", output.c_str());
  ///////////////////////////////////




  unsigned long currentMillis_SendMSG = millis();

  if (currentMillis_SendMSG - previousMillis_SendMSG >= interval_SendMSG) {
    previousMillis_SendMSG = currentMillis_SendMSG;

    Slv++;
    if (Slv > 2) Slv = 1;

    Message = "SDS" + String(Slv);

    //::::::::::::::::: Condition for sending message / command data to Slave 1 (ESP32 Slave 1).
    if (Slv == 1) {
      Serial.println();
      Serial.print("Send message to ESP32 Slave " + String(Slv));
      Serial.println(" : " + Message);
      sendMessage(salida, Destination_ESP32_Slave_1);
      client.publish("dron/MQ-8", mq8.c_str());
      client.publish("dron/MG-811", mg811.c_str());
      client.publish("dron/MQ-135", mq135.c_str());
      client.publish("dron/MQ7",  mq7.c_str());
      client.publish("dron/SDS25", sds25.c_str());
      client.publish("dron/SDS10", sds10.c_str());
      Serial.print("Publish message: ");
      Serial.println(output);
      client.publish("test", output.c_str());
    }
    //:::::::::::::::::

    //::::::::::::::::: Condition for sending message / command data to Slave 2 (UNO Slave 2).
    if (Slv == 2) {
      Serial.println();
      Serial.print("Send message to ESP32 Slave " + String(Slv));
      Serial.println(" : " + Message);
      sendMessage(Message, Destination_ESP32_Slave_2);
    }
    //:::::::::::::::::
  }
  //----------------------------------------

  //---------------------------------------- parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
