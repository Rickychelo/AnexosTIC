#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// Definir los pines del sensor
const int sensorPin = A0;

//Valores de seco y húmedo
const int dry = 700; // value for dry sensor
const int wet = 300; // value for wet sensor

// Variables de conexión WiFi
const char* ssid = "***********";
const char* password = "***********";

// Variables de conexión MQTT
const char* mqtt_server = "192.168.0.18"; // Dirección IP del servidor MQTT
const char* mqtt_topic_soil_moisture = "SoilMoistureW";


WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP:");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop hasta que estemos reconectados
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
    } else {
      // Depurar: Imprimir el estado de la conexión MQTT
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer el valor analógico del sensor
  int sensorValue = analogRead(sensorPin);
  // Convertir el valor a porcentaje de humedad
  int percentage = map(sensorValue, wet, dry, 0, 100);

  // Crear un mensaje para enviar al servidor MQTT
  String message = String(percentage);
  
  // Publicar el mensaje en el tópico MQTT
  client.publish(mqtt_topic_soil_moisture, message.c_str());

  // Measure and print WiFi signal strength (RSSI)
  long rssi = WiFi.RSSI();
  Serial.print("Intensidad de señal Wi-Fi (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");

  delay(120000); // Esperar 2 minutos antes de la siguiente lectura
}
