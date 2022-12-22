/*********************************************************
 * Asyncron UDP Server.
 * Ideen er at sende data direkte over ESP32´nes indbyggede radio via UDP.
 * Denne implementering tager udgangspunkt i den asynkrone version af UDP.h biblioteket
 * så den i praksis meget lille risiko for at miste datapakker er minimal.
 * 
 * Martin dec 2022
 */

#include <WiFi.h>
#include "AsyncUDP.h"
#include "esp_wifi.h"
 
// const char* wifi_network_ssid = "yourNetworkName";
// const char* wifi_network_password =  "yourNetworkPassword";
 
const char* soft_ap_ssid = "MyESP32AP";
const char* soft_ap_password = "testpassword";


//The udp library class
// WiFiUDP udp;
AsyncUDP udp;
 
void setup() {
 
  Serial.begin(115200);
  while (!Serial){ delay(10);}
  
  // WiFi.mode(WIFI_MODE_APSTA);
  WiFi.mode(WIFI_AP_STA);
  esp_wifi_set_protocol( WIFI_IF_AP, WIFI_PROTOCOL_LR );
  
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  //API: bool softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false);
  
  // WiFi.begin(wifi_network_ssid, wifi_network_password);
  // Serial.print("Connecting to WiFi");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(100);
  //   Serial.print(".");
  // }
  // Serial.printf("\nConnected\n");
  Serial.print("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());
 
  Serial.print("ESP32 IP on the WiFi network: ");
  // Serial.println(WiFi.localIP());
  
    if(udp.listen(1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            
            Serial.println();
            //reply to the client
            //packet.printf("Got %u bytes of data", packet.length());
        });
    }

}

char buf[64]; //allokeret 64 byte plads til i en charbuffer til metode 1 `broadcastTo()`
void loop()
{
  //Send broadcast:
  //Metode 1: null termination af strengen '\0' ved at 
  String s =  "Anyone here?"; //her 12 + 1 = 13 byte incl null termination 
  s.toCharArray(buf, sizeof(buf)); // converterer string to chararray - automatisk null termination 
  udp.broadcastTo(buf,12345);//send besked til port 12345
  delay(10);
  char mystring[9] = "Hva så?";
  delay(3000);//NB! ALDRIG delay i produktions code
    
  //Metode 2: manuel null termination af strengen - dvs tilføj til strengen'\0'
    udp.broadcastTo("123456789\0",12345);
}

