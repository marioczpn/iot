/*
  This code is running on DOIT ESP32 – ESP-WROOM-32 board 
  and also integrating with 433MHZ RF Receiver

  Written in 2018 by Mario Ponciano - marioczpn@gmail.com
*/
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <PubSubClient.h> 
#include <RCSwitch.h>
#include <IFTTTMaker.h>
#include <WiFiClientSecure.h>

RCSwitch mySwitch = RCSwitch();

//-------- your wifi -----------
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PWD";

WiFiClientSecure wifiClient;

// ------- IFTTT Maker config --------
#define KEY "IFTTT_TOKEN"  // Get it from this page https://ifttt.com/services/maker/settings
#define EVENT_NAME "front_door_opened" // Name of your event name, set when you are creating the applet

IFTTTMaker ifttt(KEY, wifiClient);

void setup() {
    Serial.begin(115200);
    connectToWifi();
    mySwitch.enableReceive(27); 
    
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    receivedRFCode();
  }
}

void connectToWifi() {
  Serial.print("Connecting to "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  } 
    
  Serial.println("");
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());  
}

void receivedRFCode() {
  unsigned int* raw;
  
  if (mySwitch.available()) { 
       raw = mySwitch.getReceivedRawdata();
       
      Serial.print("Received: ");
      for (unsigned int i=0; i <= mySwitch.getReceivedBitlength()*2; i++) {
        Serial.print(raw[i]);
        Serial.print(",");
      }
    
      long frontDoorId = 167149813;
      if(frontDoorId == mySwitch.getReceivedValue()) {
          String id = "";
          id += mySwitch.getReceivedValue();
          String doorStatus = "Front Door Opened";
          sendIFTTTNOtification(id, doorStatus);
      } else {
         Serial.print("Unknown encoding");
      }

        mySwitch.resetAvailable();
  }
  
}

void sendIFTTTNOtification(String info, String myStatus){
  //Send a IFTTT notification
  if(ifttt.triggerEvent(EVENT_NAME, info, myStatus)){
       Serial.println("[INFO] IFTTT Notification Successfully sent");
       
  } else {
    Serial.println("[INFO] IFTTT Notification failed!");
    
  }
}

