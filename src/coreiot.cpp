#include "coreiot.h"

// ----------- CONFIGURE THESE! -----------
const char* coreIOT_Server = "app.coreiot.io";  
const char* coreIOT_Token = "rqnbIzxJt7tVWT5DzzTf";   // Device Access Token
const int   mqttPort = 1883;
// ----------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect (username=token, password=empty)
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), CORE_IOT_TOKEN.c_str(), NULL)) {
        
      Serial.println("connected to CoreIOT Server!");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Subscribed to v1/devices/me/rpc/request/+");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  if (!topic || !payload || length == 0) {
    Serial.println("❌ Invalid callback parameters");
    return;
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  // Allocate buffer safely (limit to 512 bytes max)
  if (length > 512) {
    Serial.println("❌ Message too long, ignoring");
    return;
  }

  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.print("Payload: ");
  Serial.println(message);

  // Parse JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("❌ deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* method = doc["method"];
  if (!method) {
    Serial.println("❌ No method field in JSON");
    return;
  }

  if (strcmp(method, "setLedSwitchValue") == 0) {
    bool ledState = false;
    
    // Handle params as boolean or string
    if (doc["params"].is<bool>()) {
      ledState = doc["params"].as<bool>();
      Serial.print("✅ params is boolean: ");
      Serial.println(ledState ? "true (ON)" : "false (OFF)");
    } else if (doc["params"].is<const char*>()) {
      const char* params = doc["params"];
      if (strcmp(params, "ON") == 0 || strcmp(params, "true") == 0) {
        ledState = true;
      } else {
        ledState = false;
      }
      Serial.print("✅ params is string: ");
      Serial.println(params);
    } else {
      Serial.println("❌ Unknown params type");
      return;
    }
    
    if (ledState) {
      Serial.println("Device turned ON.");
      led_enable = true;
    } else {
      Serial.println("Device turned OFF.");
      led_enable = false;
    }
  } else {
    Serial.print("⚠️ Unknown method: ");
    Serial.println(method);
  }
}


void setup_coreiot(){

  //Serial.print("Connecting to WiFi...");
  //WiFi.begin(wifi_ssid, wifi_password);
  //while (WiFi.status() != WL_CONNECTED) {
  
  // while (isWifiConnected == false) {
  //   delay(500);
  //   Serial.print(".");
  // }

  while(1){
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY)) {
      break;
    }
    delay(500);
    Serial.print(".");
  }


  Serial.println(" Connected!");

  client.setServer(CORE_IOT_SERVER.c_str(), CORE_IOT_PORT.toInt());
  client.setCallback(callback);

}

void coreiot_task(void *pvParameters){

    setup_coreiot();

    while(1){

        if (!client.connected()) {
            reconnect();
        }
        client.loop();

        // Sample payload, publish to 'v1/devices/me/telemetry'
        String payload = "{\"temperature\":" + String(glob_temperature) +  ",\"humidity\":" + String(glob_humidity) + "}";
        
        client.publish("v1/devices/me/telemetry", payload.c_str());


        
        Serial.println("Published payload: " + payload);
        vTaskDelay(10000);  // Publish every 10 seconds
    }
}