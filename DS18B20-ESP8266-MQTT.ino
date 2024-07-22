#include <OneWire.h>                
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Define pin where the DS18B20 is connected
#define oneWireBus 14 // Pin D5

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

// Variables de conexión WiFi
const char* ssid = "***********";
const char* password = "***********";

// Variables de conexión MQTT
const char* mqtt_server = "192.168.0.18"; // Dirección IP del servidor MQTT
const char* mqtt_topic_temp = "TemperaturaTW";

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
  // Start serial communication for debugging
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
  // Connect to WiFi
  setup_wifi();
  // Set MQTT server
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Request temperature readings from the sensor
  sensors.requestTemperatures();
  // Fetch the temperature in Celsius
  float temperatureC = sensors.getTempCByIndex(0);
  // Print the temperature to the Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println("°C");

  // Create a message to send to the MQTT server
  String message = String(temperatureC);

  // Publish the message to the MQTT topic
  client.publish(mqtt_topic_temp, message.c_str());

  // Measure and print WiFi signal strength (RSSI)
  long rssi = WiFi.RSSI();
  Serial.print("Intensidad de señal Wi-Fi (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");

  // Add a delay between readings
  delay(120000);
}
