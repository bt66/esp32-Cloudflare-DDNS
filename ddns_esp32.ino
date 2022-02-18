#include "WiFi.h"
#include <HTTPClient.h>
 
const char* ssid = "Your Wifi SSID";
const char* password = "Yout WiFi Password";
//String backqwt = String(backquote);
String getIPpublic = "https://ifconfig.me/ip";
String CLoudflareURL ="https://api.cloudflare.com/client/v4/zones/yourZoneID/dns_records/yourRecordID";
String CloudflareXauthEmail ="email which registerd on cloudflare";
String CloudflareXauthKey ="cloudflare xauth key";
unsigned long LastTime = 0;
unsigned long timerDelay = 5000;
String response = "";
String tempIP = ""; 

HTTPClient http;

void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
}
 
void loop() {
  tempIP = response;
  if ((millis() - LastTime) > timerDelay){
    if(WiFi.status() == WL_CONNECTED){

      http.begin(getIPpublic.c_str());

      int httpResponseCode = http.GET();

      if (httpResponseCode == 200) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        response = http.getString();
        Serial.println(response);
      }
      else {
        Serial.print("Error code : ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {
      Serial.println("Wifi Disconnected");
    }
    LastTime = millis();
  }
  // check current ip same than last ip or not
  // post to cloudflare
  if (tempIP != response){
    if(WiFi.status() == WL_CONNECTED){
      http.begin(CLoudflareURL.c_str());
      http.addHeader("Content-Type", "application/json");
      http.addHeader("X-Auth-Email", CloudflareXauthEmail, false, false);
      http.addHeader("X-Auth-Key", CloudflareXauthKey, false, false);
      
      String body = "{\"type\":\"A\",\"name\":\"ddns.carakan.id\",\"ttl\":3600,\"proxied\":false,\"content\":";
      body += "\""+ response +"\"}";
      Serial.println(body);
      
      int httpResponseCode = http.PUT(body);
      
      if (httpResponseCode == 200){
        Serial.print("Cloudflare HTTP Response Code: ");
        Serial.println(httpResponseCode);
        response = http.getString();
        Serial.println(response);
      }
      else {
        Serial.print("Change Cloudflare record Error code : ");
        Serial.println(httpResponseCode);
        response = http.getString();
        Serial.println(response);
      }
      http.end();
    }
    else {
      Serial.println("Wifi Disconnected");
    }
  }
  delay(2000);
}