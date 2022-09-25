#include <WiFi.h>
#include <PubSubClient.h>

#define PIN_VOLTAGE_READ 5 // Digital pin 2 is wired to IN1 of the HY-M154
#define button_topic "esp32/button"

// Replace the next variables with your SSID/Password combination
const char* ssid = "S21+5G7aea";
const char* password = "eszw6753";


// MQTT Broker
const char* mqtt_broker = "broker.emqx.io";
const char* mqtt_username = "smartfactory";
const char* mqtt_password = "smartfactory2022";
const int mqtt_port = 1883;

//Publish topic
//const char* 

//Button
String button_value = "KACSA";

bool new_state = true;
bool last_state = true;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  pinMode(PIN_VOLTAGE_READ, INPUT_PULLUP); // Input pullup means that the signal is HIGH if circuit is switched off!
  Serial.begin(115200); // Init serial connection

  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  // client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.subscribe(button_topic);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    message  += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  /*
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }*/
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String client_id = "ESP8266Client-";
    client_id += String(random(0xffff), HEX);
    // Attempt to connect
   if (client.connect(client_id.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  last_state = new_state;
  new_state = digitalRead(PIN_VOLTAGE_READ);
  if (!new_state && last_state) {  
    // Convert the value to a char array
    // char humString[8];
   // dtostrf(humidity, 1, 2, humString);  
   Serial.println("on");  
    client.publish(button_topic, String(button_value).c_str(), true);
    // client.publish("esp32/humidity", humString);
  }


  
  
}
