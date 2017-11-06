#include <SoftwareSerial.h>
#include <WifiClient.h>
#include <rdm630.h>
#include <QueueArray.h>

// WiFi Network Definitions
// Replace these two character strings with the name and password of your WiFi network.
const char ssid[] = "JimmyWIFI";
const char password[] = "francia661944";

// Setting up objects
rdm630 rfid(11,10);
WifiClient client;
QueueArray <long> queue;

// HTTP Strings
const char destServer[] = "192.168.0.105";
const int destPort = 8888;
const String placeName = "place1";


void setup(){
  Serial.begin(112500);
  client = WifiClient(ssid, password);
  client.connect();
  rfid.begin();
}

void sendQueued(){
  if(!queue.isEmpty())
    client.get(destServer, destPort, "/api/tags?placeName="+placeName+"&tagCode="+queue.pop());
}

void addToQueue(long tag){
  if(queue.isFull()) queue.pop();
  queue.push(tag);
}

long scan(){
    byte data[6];
    byte length;

    while (true) {
      if(rfid.available()){
          rfid.getData(data,length);
          Serial.println("Data valid");
          for(int i=0;i<length;i++){
              Serial.print(data[i],HEX);
              Serial.print(" ");
          }
          Serial.println();
          //concatenate the bytes in the data array to one long which can be
          //rendered as a decimal number
          unsigned long result =
            ((unsigned long int)data[1]<<24) +
            ((unsigned long int)data[2]<<16) +
            ((unsigned long int)data[3]<<8) +
            data[4];
          Serial.print("decimal CardID: ");
          Serial.println(result);
          return result;
      }else{
        sendQueued();
      }
    }
}

void loop(){
  long tag = scan();
  int result = client.get(destServer, destPort, "/api/tags?placeName="+placeName+"&tagCode="+tag);

  //if fails, the ridden tag to the send queue
  if(result==0) addToQueue(tag);
  delay(7000);

}
