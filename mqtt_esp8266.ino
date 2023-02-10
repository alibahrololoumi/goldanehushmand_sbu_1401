/*
 alibahrololoumi
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Alibahrol"; //"aminn"
const char* password = "123456788"; //"@min2000"
const char* mqtt_server = "185.218.139.198"; //"broker.mqtt-dashboard.com" "test.mosquitto.org"  "185.218.139.198"  "test.mosquitto.org"

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
//#define MSG_BUFFER_SIZE	(50)
//char msg[MSG_BUFFER_SIZE];
//int value = 0;
String message_send_to_uno="";
String message_recived_from_uno="";
String soil="";
String water="";
String light="";
String temp="";
String hum="";
void reciever_split_monitoring();


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  randomSeed(micros());

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived ["); //Serial.print(topic);//Serial.print("] ");  
  
  for (int i = 0; i < length; i++) {
    message_send_to_uno+=(char)payload[i];//Serial.print((char)payload[i]);
  } 
  Serial.print(message_send_to_uno);//Serial.println();
  message_send_to_uno="";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect  ,"IOT","SmartHome"
    if (client.connect(clientId.c_str(),"IOT","SmartHome")) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("connectiontest", "esp8266 is connected to dashbord");
      // ... and resubscribe
      client.subscribe("recievefrombroker");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
}

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  Serial.setTimeout(100);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  reciever_split_monitoring();
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;    
    String message="sensors are ok";
    client.publish("sendtobroker", message.c_str());
    reciever_split_monitoring();
    //message=String(ali); //snprintf (msg, MSG_BUFFER_SIZE, message.c_str(), value);//Serial.print("Publish message: ");//Serial.println(message);    
  }
}
void reciever_split_monitoring(){
  if (Serial.available()) {
      message_recived_from_uno=Serial.readString();
  }
  delay(70);
  //message_recived_from_uno="99999999999999999999";
    for(int i=0;i<=4;i++){
      soil+=message_recived_from_uno[i];
    }
    client.publish("soil", soil.c_str());
    soil="";
    for(int i=5;i<=6;i++){
      water+=message_recived_from_uno[i];
    }
    client.publish("water", water.c_str());
    water="";
    for(int i=7;i<=11;i++){
      light+=message_recived_from_uno[i];
    }
    client.publish("light", light.c_str());
    light="";
    for(int i=12;i<=16;i++){
      temp+=message_recived_from_uno[i];
    }
    client.publish("temp", temp.c_str());
    temp="";
    for(int i=17;i<=21;i++){
      hum+=message_recived_from_uno[i];
    }
    client.publish("hum", hum.c_str());
    hum="";
    
    //Serial.print("message_recived_from_uno");
    //Serial.println(message_recived_from_uno);
  
}
