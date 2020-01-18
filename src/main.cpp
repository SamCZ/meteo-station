#if 0
// OPT3001 with Arduino (Lux meter)
// by Varad Kulkarni <http://www.microcontrollershub.com>
// Created 28 March 2018
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#ifdef U8X8_HAVE_HW_IC2
#include <Wire.h>
#endif

#include "meteo.h"
#include "wifi_utils.h"

const char* ssid = "CuNet";
const char* password = "abs159753";

const int UpdateInMinutes = 5;

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
Meteoradar meteo;
WiFiServer server(80);

bool hasConnectedToWiFi = false;

unsigned long lastThingSpeakUpdateTime = 0;
int rssi;
int bars;

int lastStatusCode;
String lastStatusMessage;

void print(const char* str) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0,10, str);
  u8g2.sendBuffer();

  meteo.Begin();
}

void setupWifi();

void setup() 
{
  Serial.begin(115200);
  u8g2.begin();

  print("Loading....");

  setupWifi();

  server.begin();
}

void setupWifi() {
  WiFi.config(IPAddress(192,168,0,103), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
  WiFi.begin(ssid, password);

  print("Connecting to WiFi...");

  unsigned long beginTime = millis();

  while (WiFi.status() != WL_CONNECTED) 
  {
    unsigned long nowTime = millis();

    if((nowTime - beginTime) > 30 * 1000) {
      break;
    }

    delay(500);
  }

  if(WiFi.status() == WL_CONNECTED) {
    print("Connected !");
    delay(1000);
    hasConnectedToWiFi = true;
  } else {
    print("Failed to connect to WiFi");
    delay(5000);
  }
}

void updateWifi() {
  if(!hasConnectedToWiFi) {
    return;
  }

  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }

  if(!client.available()) {
    return;
  }

  String request = client.readStringUntil('\r');

  client.flush();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  this is a must

  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><meta charset=\"UTF-8\"><title>Meteo</title></head><body>");

  client.print("Templota: ");
  client.print(meteo.GetTemperature());
  client.println(" C<br>");

  client.print("Tlak: ");
  client.print(meteo.GetPressure());
  client.println(" mPa<br>");

  client.print("Tlak nad hladinou moře: ");
  client.print(meteo.GetSealevelPressure());
  client.println(" mPa<br>");

  client.print("Metrů nad mořem: ");
  client.print(meteo.GetAltitude());
  client.println(" m<br>");

  client.print("Vlhkost vzduchu: ");
  client.print(meteo.GetHumidity());
  client.println(" %<br><br>");

  client.print("Sígnál WiFi: ");
  client.print(rssi);
  client.println(" dBi<br>");

  client.print("Síla signálu WiFi: ");
  client.print(bars);
  client.println(" / 5<br><br>");

  unsigned long timeDifference = (millis() - lastThingSpeakUpdateTime) / 1000;

  client.print("Status posledního requestu: ");
  client.print(lastStatusCode);
  client.print(" : ");
  client.print(lastStatusMessage);
  client.print(" : před ");
  client.print(timeDifference % 60);
  client.print("s ");
  client.print((int) floor(timeDifference / 60));
  client.print("m");
  client.println("<br><br>");

  /*Serial1.println("AT");
  String status = SigFox.readString();

      client.print("SigFox status: ");
      client.print(status);

      client.println("<br>");

  client.println("</body></html>");*/

  u8g2.drawBox(126, 28, 127, 29);
}

void sendGet(const String& host, uint16_t port, const String& uri) {
  HTTPClient http;
  http.begin(host, port, uri);
  int code = http.GET();
  if(code > 0) {
      u8g2.drawPixel(127, 30);
      lastStatusCode = code;
      lastStatusMessage = "OK";
  } else {
    lastStatusCode = code;
    lastStatusMessage = http.errorToString(code);
    print((String("ERR ") + host + " " + String(code)).c_str());
    delay(1000);
    print(http.errorToString(code).c_str());
  }
  http.end();
}

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

void loop() 
{
  u8g2.clearBuffer();

  if(WiFi.status() != WL_CONNECTED) {
    hasConnectedToWiFi = false;

    setupWifi();
  }

  rssi = WiFi.RSSI();
  bars = getBarsSignal(rssi);

  meteo.Update();
  meteo.WriteToDisplay(u8g2);

  if(hasConnectedToWiFi) {
    unsigned long nowTime = millis();

    unsigned long difference = nowTime - lastThingSpeakUpdateTime;

    if(lastThingSpeakUpdateTime == 0 || difference > (UpdateInMinutes * 1000 * 60)) {
      lastThingSpeakUpdateTime = nowTime;

      String data = "&field1=" + String(meteo.GetTemperature()) + "&field2=" + (String(meteo.GetPressure())) + "&field3=" + (String(meteo.GetAltitude())) + "&field4=" + (String(meteo.GetSealevelPressure())) + "&field5=" + (String(meteo.GetHumidity()));

      //sendGet(thingSpeakURL + data);
      sendGet("api.thingspeak.com", 80, "/update?api_key=O4VWJ2MLHMSLZ3VF" + data);
      //sendGet("home.novaengine.eu", 8080, "/?data=1" + data);
    }

    int size = map(difference, 0, UpdateInMinutes * 1000 * 60, 0, 128);
    u8g2.drawLine(0, 31, size, 31);
  }

  for (int b=0; b <= bars; b++) {
    u8g2.drawBox(100 + (b*5),31 - (b*3),3,b*3);
  }

  updateWifi();

  u8g2.sendBuffer();
  delay(500);
}
#endif