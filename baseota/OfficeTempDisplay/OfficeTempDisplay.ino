#include <InvertedTM1638.h>
#include <TM1638.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>


#include "FS.h"

const char* ssid = "kghome-e24";
const char* password = "renandstimpy";

#define DATA_PIN 14
#define CLK_PIN 12
#define STROBE_PIN 13

ESP8266WebServer http_server(80);
String webPage = "";
WiFiServer telnet_server(23);
#define MAX_SRV_CLIENTS 5
WiFiClient serverClients[MAX_SRV_CLIENTS];
TM1638 module(DATA_PIN, CLK_PIN, STROBE_PIN);
unsigned long a=1;

void setup() {
  Serial.begin(115200);
  Serial.print("v1.1 Connecting to station ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  SPIFFS.begin();

  http_server.on("/", []() {
    String page = webPage;
    File f = SPIFFS.open("/example.txt", "r");
    if (!f) {
       page += "file open failed<br/>";
    } else {
        //page += "File Data:";
      while(f.available()) {
        //Lets read line by line from the file
        page += f.readStringUntil('\n');
      }
      f.close();
      page += "<br/>";
    }
    Dir dir = SPIFFS.openDir("/");
    page += "<br/><ul>";
    while (dir.next()) {
      page += "<li>";
       page += dir.fileName();
       page += "</li>";
    }
    page += "</ul>";
    http_server.send(200, "text/html", page);
  });
  http_server.begin();
  Serial.println("HTTP server started");

  telnet_server.begin();
  telnet_server.setNoDelay(true);
  
  Serial1.print("Ready! Use 'telnet ");
  Serial1.print(WiFi.localIP());
  Serial1.println(" 23' to connect");

  MDNS.addService("http", "tcp", 80);  
  MDNS.addService("telnet", "tcp", 23);
}

void telnetloop(WiFiServer &server) {
  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial1.print("New client: "); Serial1.print(i);
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  //check clients for data
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
        while(serverClients[i].available()) Serial.write(serverClients[i].read());
      }
    }
  }
  //check UART for data
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        delay(1);
      }
    }
  }
}
// Define constants for left and right so we can easily choose 
// which side of the display to show our number
#define LEFT 0
#define RIGHT 1

// displayDigits[0] = 63 which displays a 0
// displayDigits[1] = 6 which displays a 1
// displayDigits[2] = 91 which displays a 2...etc
// Add 128 to value to display the same number with a dp following it.
// e.g. display a 2 with 91, display a 2. with 91+128=219
byte displayDigits[] = {63,6,91,79,102,109,124,7,127,103 };

// An array for the values to be displayed - all zeroes means show nothing.
byte values[] = { 0,0,0,0,0,0,0,0 };

// The digits which will make up a number to be displayed
// e.g. 25.63 will fill theDigits array with values of 2, 5, 6, and 3
int theDigits[] = { 0,0,0,0 };


void displayNumber(float numberToSplit, int whichSide, int numOfDPs){
 // The number to be split should be a float from 0 to 99.9999
 // If is below zero or equal to or over 100, then just display ----.
 // numOfDPs is the number of digits after the point, only 1 or 2 are acceptable values
 if(numOfDPs > 2 or numOfDPs < 1)numOfDPs = 1;

 // Extract the digits from this number.
 numberToSplit = (int)(100 * numberToSplit);
 theDigits[0] = (int)(numberToSplit/1000);
 theDigits[1] = (int)((numberToSplit - (1000*theDigits[0])) / 100);
 theDigits[2] = (int)((numberToSplit - (1000*theDigits[0]) - (100*theDigits[1]))/10);
 theDigits[3] = (int)(numberToSplit - (1000*theDigits[0]) - (100*theDigits[1]) - (10*theDigits[2]));

 // Find and store the byte variables required to show these digits
 int dispDig[4];
 if(theDigits[0] == 0) dispDig[0] = 0; // Hide a leading zero if there is one
 else dispDig[0] = displayDigits[theDigits[0]];
 dispDig[1] = displayDigits[theDigits[1]] + 128; // Apend the dp onto the second digit
 dispDig[2] = displayDigits[theDigits[2]];
 dispDig[3] = displayDigits[theDigits[3]];

 // If we are only showing one DP, then leave last character blank to make things more legible on the display
 if(numOfDPs == 1) dispDig[3] = 0;

 // Make sure that the number passed to the function was >= 0 or <100, otherwise show an error with ----.
 if(numberToSplit/100 < 0 or numberToSplit/100 >= 100){
   for(int i = 0; i < 4; i++) dispDig[i] = 64;
 }

 // Find if number to be shown on the left or the right side of the display
 int offset = 0; // LEFT by default
 if(whichSide == RIGHT) offset = 4;

 // Update the values in the values array used by the display.
 values[0+offset] = dispDig[0];
 values[1+offset] = dispDig[1];
 values[2+offset] = dispDig[2];
 values[3+offset] = dispDig[3];

 // Update the display itself with the new values.
 module.setDisplay(values);
}
void loop() {
  
  ArduinoOTA.handle();
  http_server.handleClient();
  telnetloop(telnet_server);
  displayNumber(
}
