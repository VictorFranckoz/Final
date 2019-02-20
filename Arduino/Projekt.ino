/*********************************************************************
  This is an example based on nRF51822 based Bluefruit LE modules, RFID, HC-SR04 & Arduino
  By: Mikkel From, Rasmus Lund, Daniel Qwist & Victor Franck

********************************************************************/
//Inkludering af libraries til hhv. Arduino, HC-SR04, RFID og Adafruit.
#include <Arduino.h>
#include <SPI.h>
#include <HCSR04.h>
#include <MFRC522.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif

/*=========================================================================
       -----------------------------------------------------------------------*/
#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/


Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


// HC-SR04 Defining/ Including

UltraSonicDistanceSensor distanceSensor(2, 3);  // Initialize sensor that uses digital pins 13 and 12.

//Variabler til at identificere reps.
int distance;
int lastDistance;
int thirdDistance;
int rep = 0;
int lastrep;
String sent;

/**************************************************************************/

//RFID Defining
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

//RFID Variabler
int max_kg = 0;
int last_max_kg;

/**************************************************************************/

/**************************************************************************/
//Setup Adafruit
void setup(void)
{
  while (!Serial);  // required for Flora & Micro
  delay(500);

  // Serial.begin(115200);

  Serial.begin(9600);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();



  ble.verbose(false);  // debug info is a little annoying after this point!


  Serial.println(F("******************************"));

  //Give module a new name
  ble.println("AT+GAPDEVNAME=FDMProject"); // named FDMProject
  // Check response status
  ble.waitForOK();

  /* Wait for connection */
  while (! ble.isConnected()) {
    delay(500);
  }
  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
  //RFID Scanner starter op, og begynder at søge efter RFID tags.
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan RFID Tags, for at få vist vægt.");
  Serial.println("Check tags by outcommenting Serial.println(mfrc522.uid.uidByte[0]);");
  Serial.println("Setup was completed and you are ready to lift some weights!");
}



void loop(void) {
  // Check for user input
  char n, inputs[BUFSIZE + 1];



  if (ble.available()) {
    Serial.print("* "); Serial.print(ble.available()); Serial.println(F(" bytes available from BTLE"));
  }



  // Echo received data
  while ( ble.available()) {
    int c = ble.read();
    Serial.print((char)c);

  }

  //Easy way to save last 3 measurements, for counting reps
  thirdDistance = lastDistance;
  lastDistance = distance;
  distance = distanceSensor.measureDistanceCm();

  // Serial.println("Distance" + String(distanceSensor.measureDistanceCm()));
  /*
    Serial.println("distance");


      Serial.println(String(distance));
      Serial.println(String(lastDistance));
      Serial.println(String(thirdDistance));
  */
  if (distanceSensor.measureDistanceCm() != -1) { //Has to be done since HC-SR04 value will be -1, if out of range.

    if (thirdDistance < lastDistance) { //Going up

      if (lastDistance > distance + 10) { // Going down
        //Serial.println("1 rep");
        rep = rep + 1;  //Counting reps
        //Serial.println(String(rep) + " rep"); //Printing for testing
        
        sent =String(rep)+"."+String(max_kg); //Var for ble.print.
        ble.print(sent); //Printing to app through adafruit.
        Serial.println(sent); //Printing to app through adafruit.

        delay(500);
        

      }
    }
  }

  //delay(1000);
  MFRC522::MIFARE_Key key;
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07 << 4));

  // ER DET NØDVENDIGT?!
  if ( ! mfrc522.PICC_IsNewCardPresent()) {

    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {

    return;
  }
  //HERTIL*************************


  // Dump debug info about the card. PICC_HaltA() is automatically called.
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  /*
    Dump RFID  byte0 on tag
    Serial.println(mfrc522.uid.uidByte[0]);
  */
  //Switch case, looking for specific cards, and making sure max_kg wotn go down in value.
  switch (mfrc522.uid.uidByte[0]) {
    case 165: if (max_kg < 20) max_kg = 20; break;

    case 117: if (max_kg < 30) max_kg = 30; break;

    case 62: if (max_kg < 40) max_kg = 40; break;
  
    case 52: if (max_kg < 50) max_kg = 50; break;

    case 37: if (max_kg < 60) max_kg = 60; break;

    default:
      // statements
      break;
  }
    if (mfrc522.uid.uidByte[0] == 85) { //Looking for reset tag
    max_kg = 0; //Ressetting Max_kg
    rep = 0;
    Serial.println("Reset");
  }
/*
  if (max_kg != 0) { //Looking for changes from default
    if (max_kg != last_max_kg) { //Looking for changes from last loop

      //Serial.print("Max. vægt: "); //Printing to Serial for test
      Serial.println(max_kg);
      Serial.println(last_max_kg + "Sidste");
    }
  }
  */
  last_max_kg = max_kg; //Changing last_max_kg to max_kg after each loop.
  lastrep = rep;
}
