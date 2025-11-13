#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port = 1883;

const char* sensor_topic = "healumi/sensor/ldr_value";
const char* command_topic = "healumi/light/set";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

#define LED_PIN 13
#define LED_COUNT 1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#define LDR_PIN 34

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int ldrValue = 0;
int currentHour = 0;

unsigned long mqttStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);

  strip.begin();
  strip.setBrightness(150);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  
  setup_wifi();
  
  timeClient.begin();
  Serial.println("Cliente NTP iniciado.");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.print("\nConectando ao Wi-Fi...");
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (mqttStartTime > 0) {
    unsigned long latency = millis() - mqttStartTime;
    Serial.print("Latência do Atuador: ");
    Serial.print(latency);
    Serial.println("ms ");
    mqttStartTime = 0;
  }
  
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Payload: ");
  Serial.println(message);

  int r, g, b;
  sscanf(message.c_str(), "%d,%d,%d", &r, &g, &b);

  Serial.printf("Ajustando atuador para R:%d, G:%d, B:%d\n", r, g, b);
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("HealumiWokwiClient_NTP")) { // ID do Cliente (novo)
      Serial.println("Conectado ao Broker MQTT!");
      
      client.subscribe(command_topic);
      Serial.print("Inscrito no tópico: ");
      Serial.println(command_topic);
      
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  timeClient.update();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    currentHour = timeClient.getHours();
    Serial.print("\nHora atual (UTC-3): ");
    Serial.println(currentHour);

    ldrValue = analogRead(LDR_PIN);
    Serial.print("Leitura do Sensor (LDR): ");
    Serial.println(ldrValue);

    snprintf(msg, 50, "%d", ldrValue);
    client.publish(sensor_topic, msg);
    
    int target_r, target_g, target_b;
    bool ligarLuz = false;

    if (ldrValue > 2500) {
      ligarLuz = true;
      Serial.println("Ambiente escuro, luz artificial necessária.");
    } else {
      ligarLuz = false;
      Serial.println("Ambiente claro, luz artificial não é necessária.");
    }

    if (ligarLuz) {
      if (currentHour >= 6 && currentHour < 18) {
        Serial.println("Período diurno - definindo luz fria.");
        target_r = 200;
        target_g = 200;
        target_b = 255;
      } 
      else {
        Serial.println("Período noturno - definindo luz quente.");
        target_r = 255;
        target_g = 140;
        target_b = 20;
      }
    }
    else {
      target_r = 0;
      target_g = 0;
      target_b = 0;
    }

    snprintf(msg, 50, "%d,%d,%d", target_r, target_g, target_b);
    
    mqttStartTime = millis();

    client.publish(command_topic, msg);
    Serial.print("Publicado no tópico (Comando): ");
    Serial.println(command_topic);
  }
}