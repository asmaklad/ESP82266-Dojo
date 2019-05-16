/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "yourAuthCodeFromTheBlynkEmail";

SSD1306Wire  display(0x3c, D1, D2);

String message ="";

// This function will be called every time V1 changes
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  message = param.asString(); // assigning incoming value from pin V1 to a variable
  Serial.println(message );
  if (message.length()>=0){
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    //display.drawString(0, 26, message);
    display.drawStringMaxWidth(0, 0, 128,message);
    // write the buffer to the display
    display.display();
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //
  // Initialising the UI will init the display too.
  display.init();  
  display.flipScreenVertically();
  //display.setFont(ArialMT_Plain_10);
  display.setFont(ArialMT_Plain_16);
  //display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);  
  display.clear();  
  display.drawString(0, 10, "Hello World !");
  // write the buffer to the display
  display.display();
  //
  Blynk.config(auth);
  //
}

void loop()
{
  Blynk.run();
  //display.display();
}
