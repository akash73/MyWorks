/*
  WiFi 101 ThingSpeak Data Uploader
  Hardware Required:
  Arduino Zero or Uno Board
  Arduino Wifi Sheild 101
  Photocell
  Temperature Sensor (This example uses a TMP36)
  10K Ohm Resistor

  created Sept 2015
  by Helena Bisby <support@arduino.cc>
  This example code is in the public domain
  http://arduino.cc/en/Tutorial/WiFi101ThingSpeakDataUploader

*/


#include <SPI.h>
#include <WiFi101.h>
#include "DHT.h"


// Local Network Settings
char ssid[] = "*******";  // your network SSID (name)
char pass[] = "******";    // your network password
int keyIndex = 0;             // your network key Index number (needed only for WEP)


int status = WL_IDLE_STATUS;
WiFiServer server(80);

//#define DHTPIN A0     // what pin we're connected to
//  whatever type you're using!
//#define DHTTYPE DHT22   // DHT 22  (AM2302)

//DHT dht(DHTPIN, DHTTYPE);

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String APIKey = "#########";             // enter your channel's Write API Key
const int updateThingSpeakInterval = 20 * 1000; // 20 second interval at which to update ThingSpeak

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;

// Initialize Arduino Ethernet Client
WiFiClient client;

void setup() {
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only

    //Lanuch DHT Grove sensor
    //dht.begin();
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi shield not present"));
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  // read values from pins and store as strings
  //float h = dht.readHumidity();
  //float t = dht.readTemperature();

 
  // Print Update Response to Serial Monitor
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  Serial.flush();
  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected) {
    Serial.println(F("...disconnected"));
    Serial.println();
    client.stop();
  }
  // Update ThingSpeak
  if ((millis() - lastConnectionTime > updateThingSpeakInterval) && !client.connected()) {
    updateThingSpeak("field2=" + String(2));
    //Serial.println(String(t));
    //Serial.println(String(h));

  }
  lastConnected = client.connected();
}

void updateThingSpeak(String tsData) {
  if (client.connect(thingSpeakAddress, 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + APIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);
    lastConnectionTime = millis();

    if (client.connected()) {
      Serial.println(F("Cxonnecting to ThingSpeak..."));
      Serial.println();
    }
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print(F("signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}
