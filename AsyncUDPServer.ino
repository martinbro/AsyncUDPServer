/*********************************************************
 * Asyncron UDP Server.
 * Ideen er at sende data direkte over ESP32´nes indbyggede radio via UDP.
 * Denne implementering tager udgangspunkt i den asynkrone version af UDP.h biblioteket
 * så den i praksis meget lille risiko for at miste datapakker er minimal.
 * 
 * Martin dec 2022
 */

#include "WiFi.h"
#include "AsyncUDP.h"
// #include "WiFiUdp.h" er allerede importeret i WiFi.h

const char* soft_ap_ssid = "MyESP32AP";
const char* soft_ap_password = "testpassword";

// Tildeler fast Ip-adresse til at identificere 'modulet' 
IPAddress local_IP(192, 168, 4, 1); 
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

//Definerer namespaces
WiFiUDP UDP;
AsyncUDP udp;

void setup() {
    //0. starter seriel kommunikation
    Serial.begin(115200);
    while (!Serial){ delay(10);}
  
    // 1. Sætter lokal access point op. Formålet er at kommunikere med de andre ESPér
     WiFi.mode(WIFI_AP_STA);

    // 2. sætter den faste IPadresse op defineret ovenfor 
    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
        //ESP.restart(); //Respons 'Erlang-style'
    }
    
    // 3. Starter accespoint
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
   
    // 4. lytter efter indkommen beskeder på port:1234
    if(udp.listen(WiFi.localIP(),1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        // 5. håndterer indkomne beskeder:
        udp.onPacket([](AsyncUDPPacket packet) {
            
            // Printer udvalgte metadata til testbrug
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", Data: ");
            //Den kritiske info -
            // Anvender Serial.write() da packet.data er i det bineære kode-format: uint8_t  
            // Serial.write(packet.data(), packet.length());

            if (packet.length() > 0)
            {
                char buffer[packet.length()];
                   
                String testString = String( (char*) packet.data()); 
                Serial.println(testString);
            }
            Serial.println();

            // evt auto svar/verificering til klienten:
            // packet.printf("Modtog %u bytes data", packet.length());
        });
    }
}

void loop()
{
    delay(1000);//ALDRIG delay i produktion

    //Broadcast: til alle ESPer på port: 22345
    udp.broadcastTo("Dav fra server!",22345);

    delay(1000);//ALDRIG delay i produktion
    //unicast til IP "192.168.4.3" og port 22345
    UDP.beginPacket("192.168.4.3",22345);
    UDP.print("Unicast Besked fra Esp32 server");
    UDP.endPacket();

}

