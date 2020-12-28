#define BLYNK_PRINT Serial            

#include <BlynkSimpleEsp8266.h>
    // For OTA with ESP8266
#include <WiFiUdp.h>              // For OTA
#include <ArduinoOTA.h>        // For OTA

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


BlynkTimer timer;

void checktouchButton();

int relay1State = LOW;
int touchButton1State = HIGH;

int relay2State = LOW;
int touchButton2State = HIGH;

int relay3State = LOW;
int touchButton3State = HIGH;

int relay4State = LOW;
int touchButton4State = HIGH;

#define AUTH "XefyaLMB5gzFBWQUw9wIWMp0U3mgO62s"  // You should get Auth Token in the Blynk App.  
#define WIFI_SSID "COVID-19"               //Enter Wifi Name
#define WIFI_PASS "9971944811"                    //Enter wifi Password

#define SERVER "blynk-cloud.com "              // Comment-out if use Blynk hosted cloud service
#define PORT 8442

#define RELAY_PIN_1      D1   //D6
#define RELAY_PIN_2      D2   //D0
#define RELAY_PIN_3      D3   //D2
#define RELAY_PIN_4      D4  //D1

#define Touch_Swnsor_1   D5    //D8
#define Touch_Swnsor_2   D6    //D5
#define Touch_Swnsor_3   D7    //D7
#define Touch_Swnsor_4   D8    //Rx

#define VPIN_BUTTON_1    V1
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3
#define VPIN_BUTTON_4    V4  

#ifndef STASSID
#define STASSID "COVID-19"
#define STAPSK  "9971944811"
#endif


const char* host = "aryanroom";
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

char auth[] = "XefyaLMB5gzFBWQUw9wIWMp0U3mgO62s";



void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  Blynk.begin(auth, ssid, password);
  
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(Touch_Swnsor_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);


  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(Touch_Swnsor_2, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_2, relay2State);


  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(Touch_Swnsor_3, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_3, relay3State);


  pinMode(RELAY_PIN_4, OUTPUT);
  pinMode(Touch_Swnsor_4, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_4, relay4State);

  // Setup a function to be called every 100 ms
  timer.setInterval(500L, checktouchButton);
  
  
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    MDNS.begin(host);
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex);
    });
    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    server.begin();
    MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi Failed");
  }
 
}

  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

BLYNK_CONNECTED() {

  // Request the latest state from the server

  Blynk.syncVirtual (VPIN_BUTTON_1);
  Blynk.syncVirtual (VPIN_BUTTON_2);
  Blynk.syncVirtual (VPIN_BUTTON_3);
  Blynk.syncVirtual (VPIN_BUTTON_4);

}


BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  relay2State = param.asInt();
  digitalWrite(RELAY_PIN_2, relay2State);
}
BLYNK_WRITE(VPIN_BUTTON_3) {
  relay3State = param.asInt();
  digitalWrite(RELAY_PIN_3, relay3State);
}
BLYNK_WRITE(VPIN_BUTTON_4) {
  relay4State = param.asInt();
  digitalWrite(RELAY_PIN_4, relay4State);
}



void checktouchButton()
{
  if (digitalRead(Touch_Swnsor_1) == LOW) {
    // pushButton1State is used to avoid sequential toggles
    if (touchButton1State != LOW) {

      // Toggle Relay state
      relay1State = !relay1State;
      digitalWrite(RELAY_PIN_1, relay1State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
    touchButton1State = LOW;
  } else {
    touchButton1State = HIGH;
  }

  if (digitalRead(Touch_Swnsor_2) == LOW) {
    // pushButton2State is used to avoid sequential toggles
    if (touchButton2State != LOW) {

      // Toggle Relay state
      relay2State = !relay2State;
      digitalWrite(RELAY_PIN_2, relay2State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_2, relay2State);
    }
    touchButton2State = LOW;
  } else {
    touchButton2State = HIGH;
  }

  if (digitalRead(Touch_Swnsor_3) == LOW) {
    // pushButton3State is used to avoid sequential toggles
    if (touchButton3State != LOW) {

      // Toggle Relay state
      relay3State = !relay3State;
      digitalWrite(RELAY_PIN_3, relay3State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_3, relay3State);
    }
    touchButton3State = LOW;
  } else {
    touchButton3State = HIGH;
  }

  if (digitalRead(Touch_Swnsor_4) == LOW) {
    // pushButton4State is used to avoid sequential toggles
    if (touchButton4State != LOW) {

      // Toggle Relay state
      relay4State = !relay4State;
      digitalWrite(RELAY_PIN_4, relay4State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_4, relay4State);
    }
    touchButton4State = LOW;
  } else {
    touchButton4State = HIGH;
  }
}




void loop() {
  server.handleClient();
  MDNS.update();
  Blynk.run();
  timer.run();
  
  }
