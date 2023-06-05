#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

#define RST_PIN  D0     // Configurable, see typical pin layout above
#define SS_PIN   D8    // Configurable, see typical pin layout above
#define pin      D1


MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;  
//ESP8266WiFiMulti WiFiMulti;
MFRC522::StatusCode status;      


const char* ssid     = "realme 7";
const char* password = "1234567890";

// Enter Google Script Deployment ID:
const char *GScriptId = "AKfycbzoPeUk5dk6UsQAu3OcvtgDHKklk-qm1zbqpBAKisyKDa-EYZEj7E53abKLtwbZ_K6SCQ";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;


byte nuidPICC[4]; 


void setup() 
{
  Serial.begin(9600);
  SPI.begin();

  pinMode(pin,OUTPUT);
  digitalWrite(pin,LOW);   
  delay(10);
  Serial.println('\n');
  
  // Connect to WiFi
  WiFi.begin(ssid, password);             
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<6; i++){ 
    int retval = client->connect(host, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Connected");
       break;
    }
    else if(i == 5)
    {
      Serial.println("Restarting the module");
      ESP.restart();
      }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object

  rfid.PCD_Init();
  for(byte i =0 ;i<6;i++)
  {
    key.keyByte[i] = 0xFF;
    }

    Serial.println("Scan your card");
}

void loop()
{

  
  
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  if ( ! rfid.PICC_ReadCardSerial())
    return;

  for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }


    Serial.println("Card Scaned");
    
    switch(check(rfid.uid.uidByte))
    {
      case 0 : mech();
      upload("yatharth","","");
               break;
      case 1 : mech();
      upload("shubham","","");;
              
               break; 
                      
      case 2 : mech();
      upload("Swati","","");
               
               break;
      case 3 : mech();
      upload("Rohit","","");
                              break;         
      case 4 : mech(); 
      upload("Devid","","");
               
               break;         
      
      }

  Serial.println("wait");
    
  for(int i = 0; i< 10 ; i++)
  {
    delay(1000);
    Serial.print(" ");
    Serial.print(i);
    
    }

    Serial.println();
    Serial.println("Scan your card");
  
  }







int check(byte *buff)
{
  int a = 0;
  byte cred[5][4]={{0x59,0x91,0x2E,0x7D},{0x13,0x39,0x8A,0x46},{0xEC,0x98,0x4D,0x7F},{0x59,0x91,0x2E,0x72},{0x59,0x91,0x2E,0x71}};
  for(int j = 0;j<5;j++)
  {
    for(int i = 0;i<4;i++)
  {
    if(buff[i]== cred[j][i])
    {a++;}
  }
  if(a==4)
  return j;
  else if(j == 4)
  return 5;
  else 
  {a=0;}  
    }
}
  

void upload(String a , String b, String c)
{
  // create some fake data to publish
//  value0 ++;
//  value1 = random(0,1000);
//  value2 = random(0,100000);

  Serial.println("Got your Request");
  static bool flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + a + "," + b + "," + c + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){ 
    // do stuff here if publish was successful
  }
  else{
    // do stuff here if publish was not successful
    Serial.println("Error while connecting");
  }  
  
  }


void mech()
{
  digitalWrite(pin,HIGH);
  delay(5000);
  digitalWrite(pin,LOW);
  delay(5000);
  
  }