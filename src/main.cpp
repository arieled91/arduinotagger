#include <SoftwareSerial.h>
#include <WifiClient.h>
#include <rdm630.h>
// #include <QueueArray.h>

// WiFi Network Definitions
// Replace these two character strings with the name and password of your WiFi network.
const char SSID[] = "JimmyWIFI";
const char PASSWORD[] = "francia661944";

// HTTP Strings
const char destServer[] = "192.168.0.105";
const int destPort = 8888;
const String placeName = "place1";


// Variables and Objects
rdm630 rfid(11,10);
WifiClient client;
// QueueArray <long> queue;
unsigned long previous = 0;
unsigned long prevTime = millis();
const int COOLDOWN_TIME_MILLIS = 10000;




void setup(){
  Serial.begin(112500);
  client = WifiClient(SSID, PASSWORD);
  while(client.connect()==0);
  rfid.begin();
  // queue.setPrinter(Serial);
  Serial.println("Listening to RFID...");
}

// void sendQueued(){
//   if(!queue.isEmpty()){
//     Serial.println("Sending queue...");
//     long next = queue.front();
//     int status = client.get(destServer, destPort, "/api/tags?placeName="+placeName+"&tagCode="+next);
//     if(status==1) queue.pop();
//
//   }
// }

// void addToQueue(long tag){
//   if(queue.isFull()) queue.pop();
//   queue.push(tag);
// }

long getCardId(){
  byte data[6];
  byte length;

  rfid.getData(data,length);

  unsigned long result =
     ((unsigned long int)data[1]<<24) +
     ((unsigned long int)data[2]<<16) +
     ((unsigned long int)data[3]<<8) +
     data[4];

   return result;
}

void send(long cardId){
  Serial.print("CardID: ");
  Serial.println(cardId);
  client.get(destServer, destPort, "/api/tags?placeName="+placeName+"&tagCode="+cardId);
  previous = cardId;
  prevTime = millis();
}

bool cooldownHasFinished(){
  return (millis()-prevTime)>COOLDOWN_TIME_MILLIS;
}

void loop(){
  if(rfid.available()){
    unsigned long cardId = getCardId();

    if(cardId!=previous || cooldownHasFinished())
      send(cardId);
  }
}
