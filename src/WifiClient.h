#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>


class WifiClient {
private:
String ssid;
String password;
int rx=0;
int tx=0;
void checkConnection();
static String buildRequest(String method, String path, String args);
static void errorMsg(int error);

public:
WifiClient();
WifiClient(String ssid, String password);
int connect();
int get(String server, int port, String path);
void displayConnectInfo();
void factoryReset();
void initialize();
};

WifiClient::WifiClient(){}

WifiClient::WifiClient(String ssid, String password)
{
        this->ssid = ssid;
        this->password = password;
        // factoryReset();
        initialize();
}

int WifiClient::connect()
{
  Serial.begin(115200);
  // The ESP8266 can be set to one of three modes:
  //  1 - ESP8266_MODE_STA - Station only
  //  2 - ESP8266_MODE_AP - Access point only
  //  3 - ESP8266_MODE_STAAP - Station/AP combo
  // Use esp8266.getMode() to check which mode it's in:
  int retVal = esp8266.getMode();
  if (retVal != ESP8266_MODE_STA)
  { // If it's not in station mode.
    // Use esp8266.setMode([mode]) to set it to a specified
    // mode.
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if (retVal < 0)
    {
      Serial.println(F("Error setting mode."));
      errorMsg(retVal);
    }
  }
  Serial.println(F("Mode set to station"));

  // esp8266.status() indicates the ESP8266's WiFi connect
  // status.
  // A return value of 1 indicates the device is already
  // connected. 0 indicates disconnected. (Negative values
  // equate to communication errors.)
  retVal = esp8266.status();
  if (retVal <= 0)
  {
    Serial.print(F("Connecting to "));
    Serial.println(ssid);
    // esp8266.connect([ssid], [psk]) connects the ESP8266
    // to a network.
    // On success the connect function returns a value >0
    // On fail, the function will either return:
    //  -1: TIMEOUT - The library has a set 30s timeout
    //  -3: FAIL - Couldn't connect to network.
    retVal = esp8266.connect(ssid.c_str(), password.c_str());
    if (retVal < 0)
    {
      Serial.println(F("Error connecting"));
      errorMsg(retVal);
      return 0;
    }
  }

  displayConnectInfo();
  return 1;
}

void WifiClient::factoryReset(){
  esp8266.println("AT+RESTORE");
}

void WifiClient::initialize()
{
  // esp8266.begin() verifies that the ESP8266 is operational
  // and sets it up for the rest of the sketch.
  // It returns either true or false -- indicating whether
  // communication was successul or not.
  // true
  //esp8266.println("AT+RESTORE"); //uncomment this to do a factory reset

  Serial.begin(115200);
  int test = esp8266.begin();
  if (test != true)
  {
    Serial.println(F("Error talking to ESP8266."));
    errorMsg(test);
  }
  Serial.println(F("ESP8266 Shield Present"));
}

void WifiClient::displayConnectInfo()
{
  char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  // esp8266.getAP() can be used to check which AP the
  // ESP8266 is connected to. It returns an error code.
  // The connected AP is returned by reference as a parameter.
  int retVal = esp8266.getAP(connectedSSID);
  if (retVal > 0)
  {
    Serial.print(F("Connected to: "));
    Serial.println(connectedSSID);
  }

  // esp8266.localIP returns an IPAddress variable with the
  // ESP8266's current local IP address.
  IPAddress myIP = esp8266.localIP();
  Serial.print(F("My IP: ")); Serial.println(myIP);
}

// errorMsg prints an error code
void WifiClient::errorMsg(int error)
{
  Serial.print(F("Error: ")); Serial.println(error);
}

void WifiClient::checkConnection(){
  esp8266.updateStatus();
  if(esp8266.status()==0) connect();
}

int WifiClient::get(String server, int port, String path){
    Serial.begin(115200);
    checkConnection();
    Serial.println("Connecting to server...");

    const String httpRequest = "GET "+path+" HTTP/1.1\n"
                               "Host: "+esp8266.localIP()+"\n"
                               "Connection: keep-alive\n\n";

    // To use the ESP8266 as a TCP client, use the
    // ESP8266Client class. First, create an object:
    ESP8266Client client;

    // ESP8266Client connect([server], [port]) is used to
    // connect to a server (const char * or IPAddress) on
    // a specified port.
    // Returns: 1 on success, 2 on already connected,
    // negative on fail (-1=TIMEOUT, -3=FAIL).
    int retVal = client.connect(server, port);
    if (retVal <= 0)
    {
      Serial.println(F("Failed to connect to server."));
      return;// To use the ESP8266 as a TCP client, use the
    // ESP8266Client class. First, create an object:
    ESP8266Client client;

    // ESP8266Client connect([server], [port]) is used to
    // connect to a server (const char * or IPAddress) on
    // a specified port.
    // Returns: 1 on success, 2 on already connected,
    // negative on fail (-1=TIMEOUT, -3=FAIL).
    int retVal = client.connect(server, port);
    if (retVal <= 0)
    {
      Serial.println(F("Failed to connect to server."));
      return 0;
    }
    }

    // print and write can be used to send data to a connected
    // client connection.
    client.print(httpRequest);

    // available() will return the number of characters
    // currently in the receive buffer.
    while (client.available())
      Serial.write(client.read()); // read() gets the FIFO char

    // connected() is a boolean return value - 1 if the
    // connection is active, 0 if it's closed.
    // if (client.connected())
    //   client.stop(); // stop() closes a TCP connection.
    return 1;
  }

  void displayConnectInfo()
  {
    char connectedSSID[24];
    memset(connectedSSID, 0, 24);
    // esp8266.getAP() can be used to check which AP the
    // ESP8266 is connected to. It returns an error code.
    // The connected AP is returned by reference as a parameter.
    int retVal = esp8266.getAP(connectedSSID);
    if (retVal > 0)
    {
      Serial.print(F("Connected to: "));
      Serial.println(connectedSSID);
    }

    // esp8266.localIP returns an IPAddress variable with the
    // ESP8266's current local IP address.
    IPAddress myIP = esp8266.localIP();
    Serial.print(F("My IP: ")); Serial.println(myIP);
  }


String WifiClient::buildRequest(String method, String path, String args){
  String request = method+" "+path;
  if(args.length()>0) request = request + "?"+args;
  return request+" HTTP/1.1\r\n";
}
