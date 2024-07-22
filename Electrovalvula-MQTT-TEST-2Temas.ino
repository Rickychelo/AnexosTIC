// Librerías
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configuración Wi-Fi
const char* ssid = "***********";
const char* password = "***********";

// Configuración MQTT
const char* mqtt_server = "192.168.0.24"; //Dirección del MQTT
const int mqtt_port = 1883; 
const char* topic_activar = "activarValvula";
const char* topic_desactivar = "desactivarValvula";

// Pin del relé
#define RELAY_PIN 12

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensaje recibido en tópico [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (String(topic) == topic_activar) {
    if (message == "ACTIVAR") {
      activarValvula();
    }
  } else if (String(topic) == topic_desactivar) {
    if (message == "DESACTIVAR") {
      desactivarValvula();
    }
  }
}

void activarValvula() {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Electroválvula activada");
}

void desactivarValvula() {
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Electroválvula desactivada");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Conectado");
      client.subscribe(topic_activar);
      client.subscribe(topic_desactivar);
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
