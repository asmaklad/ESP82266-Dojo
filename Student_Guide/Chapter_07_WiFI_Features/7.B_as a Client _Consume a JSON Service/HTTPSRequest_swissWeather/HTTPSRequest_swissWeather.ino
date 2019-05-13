/*
    HTTP over TLS (HTTPS) example sketch

    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.

    Limitations:
      only RSA certificates
      no support of Perfect Forward Secrecy (PFS)
      TLSv1.2 is supported since version 2.4.0-rc1

    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <ESP8266mDNS.h>   

#include <ArduinoJson.h>

const char* host = "opendata.netcetera.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
// from firefox: press the green lock on the addressbar next to the Https:// url and view the Certificate, look for SHA1 "Fingerprint"
// from chrome: press the lock on the addressbar next to the Https:// url look for certificate details and you will find it under the name "Thumbprint"
const char fingerprint[] PROGMEM = "63e90ab49c379653aed3d0c5b5e10bddbd9a05f3";

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("Signal Strength: ");
  Serial.println(WiFi.RSSI());              
  Serial.print("AP MAC Addr.: ");
  //Serial.print(WiFi.BSSID());  
  Serial.print("\t");
  Serial.println(WiFi.BSSIDstr()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());   
  Serial.print("MAC Address:\t");
  Serial.println(WiFi.macAddress());   
  Serial.print("SerialID:\t");
  Serial.println(ESP.getChipId());
  
  // to make sure the AP is disabled
  // not really necessary when using WifiManager
  WiFi.mode(WIFI_STA);
  WiFi.softAPdisconnect(false); 
  WiFi.enableAP(false); 
  
  ///
  if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
}

void loop() {
  Serial.println("--------------------------------------------");
  ///
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;  
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  //Rest API swgger doc : https://opendata.netcetera.com/smn/swagger#!/smn/getSmnRecord_get_2
  String url = "/smn/smn/BER";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"station\":{\"code\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");

  // get this from https://arduinojson.org/v5/assistant/ by pasting the JSON sample into the website.
  const size_t capacity = JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(13) + 330;
  DynamicJsonBuffer jsonBuffer(capacity);
  
  const char* json = line.c_str();
  
  JsonObject& root = jsonBuffer.parseObject(json);
  
  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  JsonObject& station = root["station"];
  const char* station_code = station["code"]; // "LUG"
  const char* station_name = station["name"]; // "Lugano"
  long station_ch1903Y = station["ch1903Y"]; // 717873
  long station_ch1903X = station["ch1903X"]; // 95884
  int station_lat = station["lat"]; // 46
  float station_lng = station["lng"]; // 8.966667
  int station_elevation = station["elevation"]; // 273
  
  const char* code = root["code"]; // "LUG"
  const char* dateTime = root["dateTime"]; // "2019-05-12T00:50:00.000Z"
  const char* temperature = root["temperature"]; // "13.2"
  const char* sunshine = root["sunshine"]; // "0"
  const char* precipitation = root["precipitation"]; // "0.0"
  const char* windDirection = root["windDirection"]; // "14"
  const char* windSpeed = root["windSpeed"]; // "9.7"
  const char* qnhPressure = root["qnhPressure"]; // "1013.0"
  const char* gustPeak = root["gustPeak"]; // "20.9"
  const char* humidity = root["humidity"]; // "44"
  const char* qfePressure = root["qfePressure"]; // "977.4"
  const char* qffPressure = root["qffPressure"]; // "1012.9"

  // Print values.
  Serial.print("station_name:");  
  Serial.println(station_name);
  Serial.print("temperature:");  
  Serial.println(temperature);  
  Serial.print("humidity:");  
  Serial.println(humidity);  

  delay(1000*60*10);
}
