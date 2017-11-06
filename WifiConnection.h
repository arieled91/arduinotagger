#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>

SoftwareSerial wifiSerial(0,0);
const String server = "google.com";

class WifiConnection {
private:
String ssid;
String password;
int rx=0;
int tx=0;
int status=0;

public:
WifiConnection();
WifiConnection(int rx, int tx, String ssid, String password);
int connect();
int getStatus();
};

WifiConnection::WifiConnection(){}

WifiConnection::WifiConnection(int rx, int tx, String ssid, String password)
{
        this->ssid = ssid;
        this->password = password;
        this->tx = tx;
        this->rx = rx;
        this->status = 0;
}

// Find the number of beats the button has been down for. Function can compute max. 8 sec down before rolling over.
int WifiConnection::connect()
{
  SoftwareSerial newWifiSerial = SoftwareSerial(rx,tx);
  Serial.println("Starting WIFI Connection");

  Serial.begin(115200);
  newWifiSerial.begin(115200); //115200
  newWifiSerial.println("AT");
  delay(2000);
  if (newWifiSerial.find("OK"))
    Serial.println("AT response OK");
  else
    Serial.println("Error in ESP8266");

  //-----Configuración de red-------//

  //ESP8266 en modo estación (nos conectaremos a una red existente)
  newWifiSerial.println("AT+CWMODE=3");
  if (newWifiSerial.find("OK"))
    Serial.println("ESP8266 in Station and Access-Point mode");

  //Nos conectamos a una red wifi
  newWifiSerial.print("AT+CWJAP=");
  newWifiSerial.println("\""+ssid+"\",\""+password+"\"");

  Serial.println("Connecting to '"+ssid+"'...");
  newWifiSerial.setTimeout(10000); //Aumentar si demora la conexion
  newWifiSerial.println("AT+CIPMUX=0");
  if (newWifiSerial.find("OK")){
    wifiSerial = newWifiSerial;
    Serial.println("Wifi Connected");
    status = 1;
  }else{
    status = 0;
    Serial.println("Connection error :(");
}
  return status;

  //------fin de configuracion WiFi-------------------
  newWifiSerial.print("AT+CIPSTART=\"TCP\",\"");
  newWifiSerial.print(server);
  newWifiSerial.println("\",80");

}

int WifiConnection::getStatus(){
  return status;
}
