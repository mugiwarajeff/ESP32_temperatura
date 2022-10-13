#include "DHT.h"

//Json
#include "ArduinoJson.h"
char temperaturePub[100];
char humidityPub[100];

//definindo constantes do DHT
#define DHTPIN 4
#define DHTTYPE DHT11
#define LEDPIN 19
DHT dht(DHTPIN, DHTTYPE);

//Definindo dados Wifi
#include "WiFi.h"
const char* ssid = "Jefferson";
const char* passwordWifi = "gelson43";
WiFiClient esp32Client;
#define WIFILEDPIN 2

//Definindo dados MQTT
#include "PubSubClient.h" 
const char* mqttBroker = "mqtt.tago.io";
const int mqttBrokerPort = 1883;
const char* username = "default";
const char* password = "ecb20fe8-f41c-4f10-b1b3-033be47be750";
const char* temperatureTopic = "ESP32_Jefferson/temperature";
const char* humidityTopic = "ESP32_Jefferson/humidity";
PubSubClient client(esp32Client);



void setup() {
  Serial.begin(115200);
  Serial.println("Teste de temperatura");

  //IniciandoPinoLed
  pinMode(LEDPIN, OUTPUT);

  //Inicianlizando Sensor DHT
  dht.begin();

  //Inicianlizando wifi e pinostatus
  connectWifi(); 
  pinMode(WIFILEDPIN, OUTPUT);

  //Inicializando servidor MQTT
  client.setServer(mqttBroker, mqttBrokerPort);
  //client.setCallback(callback);
}

void loop() {
  if (!client.connected()){
    reconnectMQTT();
    }
  getDHTData();
  publishTemperatureHumidity();
  showWifi();
}

void getDHTData(){
  float temperature = dht.readTemperature(false);
  float humidity = dht.readHumidity();

  digitalWrite(LEDPIN, LOW);
  delay(1000);

  if (isnan(temperature) || isnan(humidity)){
    Serial.println("Valores näo puderam ser lidos...");
  }else{

    Serial.println("=================================");
    Serial.println("=================================");
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("Kg/M");

  }

  digitalWrite(LEDPIN, HIGH);
  delay(5000);
 }

 //funcoes auxiliares wifi
 void connectWifi(){
    WiFi.begin(ssid, passwordWifi);

    while(WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.println("Connecting to WIFI...");
      }

    Serial.println("Wifi connected");
    digitalWrite(WIFILEDPIN, HIGH);
  }

  void showWifi(){
    Serial.println(WiFi.status());
    }

  //funcoes auxiliares MQTT
  void reconnectMQTT(){
    while(!client.connected()){
      client.connect("esp32Client", username, password);
      Serial.println(client.state());
    }
  }

  void publishTemperatureHumidity(){
    float temperature = dht.readTemperature(false);
    float humidity = dht.readHumidity();

    //declarando Jsons
    StaticJsonDocument<300> temperatureJson;
    StaticJsonDocument<300> humidityJson;
    
    humidityJson["variable"] = "Humidity";
    humidityJson["value"] = humidity;
    temperatureJson["variable"] = "Temperature";
    temperatureJson["value"] = String(temperature);

    serializeJson(humidityJson, humidityPub);
    serializeJson(temperatureJson, temperaturePub);
 
    client.publish(temperatureTopic, temperaturePub, true);
    client.publish(humidityTopic, humidityPub, true);
      
   
    }
