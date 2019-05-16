

/*
Name:        inlineKeyboard.ino
Created:     29/05/2018
Author:      Stefano Ledda <shurillu@tiscalinet.it>
Description: a simple example that do:
             1) if a "show keyboard" text message is received, show the inline custom keyboard, 
                otherwise reply the sender with "Try 'show keyboard'" message
             2) if "LIGHT ON" inline keyboard button is pressed turn on the onboard LED and show an alert message
             3) if "LIGHT OFF" inline keyboard button is pressed, turn off the onboard LED and show a popup message
             4) if "see docs" inline keyboard button is pressed, 
                open a browser window with URL "https://github.com/shurillu/CTBot"
*/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>     
//
#include "CTBot.h"


#define LIGHT_ON_CALLBACK  "lightON"  // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "lightOFF" // callback data sent when "LIGHT OFF" button is pressed
#define LIGHT_ON_WITH_DURATION "LIGHT_ON_WITH_DURATION"
#define LIGHT_ON_SET_DURATION  "LIGHT_ON_SET_DURATION"
#define GET_CHAT_ID_CALLBACK "GET_CHAT_ID_CALLBACK"

CTBot myBot;
CTBotInlineKeyboard myKbd;  // custom inline keyboard object helper

boolean bfSetDuration=false;
int duration=30;

String token = "<<>>"   ; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
//int AdminChatID=<<our own chatid>;
uint8_t led = BUILTIN_LED;            // the onboard ESP8266 LED.    
                            // If you have a NodeMCU you can use the BUILTIN_LED pin
                            // (replace 2 with BUILTIN_LED) 
                            // ATTENTION: this led use inverted logic

void setup() {
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  // connect the ESP8266 to the desired access point
  //myBot.wifiConnect(ssid, pass);
  // set the telegram bot token
  myBot.setTelegramToken(token);

  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");

  // set the pin connected to the LED to act as output pin
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); // turn off the led (inverted logic!)
}

void createInlineKB(){
  myKbd.flushData();
  // inline keyboard customization
  // add a query button to the first row of the inline keyboard
  myKbd.addButton("LIGHT ON", LIGHT_ON_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  // add another query button to the first row of the inline keyboard
  myKbd.addButton("LIGHT OFF", LIGHT_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  // add a new empty button row  
  myKbd.addRow();  
  char sptrUserQuestion[100];
  sprintf (sptrUserQuestion,"LIGHT ON (%d)sec",duration);
  myKbd.addButton(sptrUserQuestion, LIGHT_ON_WITH_DURATION, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("SET DURATION", LIGHT_ON_SET_DURATION, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("see docs", "https://github.com/shurillu/CTBot", CTBotKeyboardButtonURL);  
  myKbd.addRow();
  myKbd.addButton("GET CHAT ID", GET_CHAT_ID_CALLBACK, CTBotKeyboardButtonQuery);
}

void switchON(TBMessage msg){
  // pushed "LIGHT ON" button...
  digitalWrite(led, LOW); // ...turn on the LED (inverted logic!)
  // terminate the callback with an alert message
  myBot.sendMessage(msg.sender.id, "Light is now ON");
  if (msg.sender.id!=AdminChatID){
    char sptrUserQuestion[100];
    sprintf (sptrUserQuestion,"Light is now ON by: %s / %d", msg.sender.username.c_str(), msg.sender.id);
    myBot.sendMessage(AdminChatID, sptrUserQuestion );
  }
  myBot.endQuery(msg.callbackQueryID, "Light on", true);
  bfSetDuration=false;
}

void switchOFF(TBMessage msg){
  // pushed "LIGHT OFF" button...
  digitalWrite(led, HIGH); // ...turn off the LED (inverted logic!)
  // terminate the callback with a popup message
  myBot.sendMessage(msg.sender.id, "Light is now OFF");
  if (msg.sender.id!=AdminChatID){
    char sptrUserQuestion[100];
    sprintf (sptrUserQuestion,"Light is now OFF by: %s / %d", msg.sender.username.c_str(), msg.sender.id);
    myBot.sendMessage(AdminChatID, sptrUserQuestion ); 
  }
  myBot.endQuery(msg.callbackQueryID, "Light off");
  bfSetDuration=false;
}

void loop() {
  // a variable to store telegram message data
  TBMessage msg;
  // if there is an incoming message...
  //if (myBot.getNewMessage(msg) && (msg.sender.id==AdminChatID || msg.sender.id==<<OtherChatID>>)) {
  if (myBot.getNewMessage(msg)) {
    // check what kind of message I received
    if (msg.messageType == CTBotMessageText) {
      // received a text message
      if (msg.text.equalsIgnoreCase("show keyboard")) {
        // the user is asking to show the inline keyboard --> show it
        createInlineKB(); 
        myBot.sendMessage(msg.sender.id, "Inline Keyboard", myKbd);
      }else if (bfSetDuration){
         int dur=msg.text.toInt();
         if (dur>0){
          duration=dur;
          myBot.sendMessage(msg.sender.id, String("Duration is now (sec):")+duration);
         }else{
          myBot.sendMessage(msg.sender.id, String("Invalid Number:")+duration);
         }
         bfSetDuration=false;
      }else {
        // the user write anithing else --> show a hint message
        myBot.sendMessage(msg.sender.id, "Try 'show keyboard'");
        bfSetDuration=false;
      }
    } else if (msg.messageType == CTBotMessageQuery) {
      // received a callback query message
      if (msg.callbackQueryData.equals(LIGHT_ON_CALLBACK)) {
        switchON(msg);
      } else if (msg.callbackQueryData.equals(LIGHT_OFF_CALLBACK)) {
        switchOFF(msg);
      } else if (msg.callbackQueryData.equals(LIGHT_ON_WITH_DURATION)) {
        switchON(msg);
        delay(duration*1000);
        switchOFF(msg);
      } else if (msg.callbackQueryData.equals(LIGHT_ON_SET_DURATION)) {
        char sptrUserQuestion[100];
        sprintf (sptrUserQuestion,"How Much Duration in Seconds ? currently[ %d ]",duration);
        myBot.sendMessage(msg.sender.id, sptrUserQuestion);
        bfSetDuration=true;
      } else if (msg.callbackQueryData.equals(GET_CHAT_ID_CALLBACK)) {
        char sptrUserQuestion[100];
        sprintf (sptrUserQuestion,"Your CHAT_ID: %d\nYour USERNAME: %s" ,msg.sender.id ,msg.sender.username.c_str());
        myBot.sendMessage(msg.sender.id, sptrUserQuestion);
        bfSetDuration=false;
      }
    }
  }
  // wait 500 milliseconds
  delay(500);
}
