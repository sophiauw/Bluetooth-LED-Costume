/* This code is a modified version of Adafruit's nRF51822 Bluefruit LE controller code
 * I modified the code to control LED Fairy lights with 4 different settings. 
 * I was only interested in the control pad portion of the code and did not include the other portion of the code. 
 */
/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by 
 purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         0
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN); Not used 

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */ 
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Using ble with hardware SPI

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];

// Define pin numbers
const int LED_1 = 6;     //LED row 1 in dress. Since the LED string has alternating LEDs, the Row 1 is + for the odd LEDs, - for even LEDs.  
const int LED_2 = 5;     //LED row 2 in dress. Since the LED string has alternating LEDs, the Row 2 is + for the even LEDs, - for odd LEDs.  
const int LED_3 = 9;     //LED in crown
const int enablePin = 3;    // H-bridge enable pin

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int lastButtonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

// Set up integers
int animationState = 1; 
int buttnum = 1;
int animationStateN =1;



/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
 /**************************************************************************/
/*!
    Pin modes
*/
/**************************************************************************/
pinMode(enablePin, OUTPUT); // Sets the H bridge enable pin as an output
pinMode(LED_3, OUTPUT); // Sets the crown LED string to an output. Unlike the other LED strings, these remain as an output

/**************************************************************************/
/*!
    BLE setup
*/
/**************************************************************************/
//  while (!Serial);  // required for Flora & Micro ! If not using Flora or Micro comment out, otherwise code will only work while connected to the computer!!!!!
//  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit App Controller Example"));
  Serial.println(F("-----------------------------------------"));

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
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }


  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));

}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  /**************************************************************************/
/*!
    Read packet
*/
/**************************************************************************/
  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT); // Read packed from bluetooth controller app
 // if (len == 0) return;

  /* Got a packet! */
  // printHex(packetbuffer, len);

 /**************************************************************************/
/*!
    Determening if/what button is pressed and setting animations states
*/
/**************************************************************************/
 //Serial.println(packetbuffer[2] -'0'); // used for debugging
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0'; // The button number being pressed on the controller
    boolean pressed = packetbuffer[3] - '0';  
    Serial.print ("Button "); Serial.print(buttnum); // Print buttonum
    if (pressed) {
      Serial.println(" pressed"); //If pressed print pressed
      } 
    else {
      Serial.println(" released"); // If released print released
      }
      /***********************************************************
      Relating button numbers to animation states of LEDs
      **********************************************************/
    // If button num is 1, animation state is contition 1
    if (buttnum ==1){ 
      animationState=1; 
      animationStateN=animationState;
      }

      // If button num is 2, animation state is condition 2
    else if (buttnum==2){ 
      animationState=2;
      animationStateN=animationState;
      }
      // If button num is 3, animation state is condition 3
     else if (buttnum==3){ 
        animationState=3;
        animationStateN=animationState;
        }

        // If button num is 4, animation state is condition 4
      else if (buttnum==4){ 
        animationState=4;
        animationStateN=animationState;
        }
        }

    // If nothing has been pressed, condition remains the same and animation state = the current animation state.
    // The code is written this way insetead of a while loop so that the readpacket is only included once and not within each animation state with a break condition.  
    else{
      animationState = animationStateN;
      }
    
    //Serial.println(packetbuffer[2] -'0'); // used for debugging

/**************************************************************************/
/*!
    animationState == 0 
    Lights are off
*/
/**************************************************************************/

if (animationState == 1)
 {
  digitalWrite(enablePin, LOW); // Sets H bridge off
  //turn on LED L1
  pinMode(LED_1, OUTPUT);     //Row 1 is an output
  digitalWrite(LED_1, LOW);   //Row 1 is low
  pinMode(LED_2, OUTPUT);     //Row 2 is an output
  digitalWrite(LED_2, LOW);   //Row 2 is low
  digitalWrite(LED_3, LOW);   //Row 3 is low
  }

/**************************************************************************/
/*!
    animationState == 2
    Light complete a slow fade
*/
/**************************************************************************/
else if(animationState == 2) //Slow Fade atlernating
 {
// set enablePin high so that LEDs turn on:
    digitalWrite(enablePin, HIGH);
    
//turn on LED L1

  pinMode(LED_1, OUTPUT);     //row 1 is output
  digitalWrite(LED_1, LOW);  // row 1 is low
  pinMode(LED_2, OUTPUT);     //row 2 output
  digitalWrite(LED_2, HIGH);  // row 2 is high  
  int x = 5;
   for (int i = 0; i > -1; i = i + x){ // Starting with i = 0 for i> -1, i increments by x which is +-5 depending on direction
  //Serial.println( i); // used for debugging
      analogWrite(enablePin, i); // Set the motor controller pin to a value of i
      analogWrite(LED_3, i); // Set crown LEDs to i so they are in synch with the dress
      if (i == 255) x = -5;   // When i is 255, start to fade out
      if (i==255) delay(1000); // switch direction at peak and wait 1 second so that full light is visible for a longer period of time
      delay(30);
    }
  delay(10);
  
//turn on LED L2

  pinMode(LED_1, OUTPUT);     //row 1 is output
  digitalWrite(LED_1, HIGH); //row 1 is high
  pinMode(LED_2, OUTPUT);     //row 2 is output
  digitalWrite(LED_2, LOW);   // row 2 is low
 int y = 5;
   for (int i = 0; i > -1; i = i + y){ // Starting with i = 0 for i> -1, i increment by y which is +-5 depending on direction
      analogWrite(enablePin, i); // Set the motor controller pin to a value of i
      analogWrite(LED_3, i); // Set crown LEDs to i so they are in synch with the dress
      if (i == 255) y  = -5; // When i is 255, start to fade out
       if (i==255) delay(1000);// switch direction at peak and wait 1 second so that full light is visible for a longer period of time
      delay(30);
   }
  delay(10);
 
 }

/**************************************************************************/
/*!
    animationState == 3
    Light complete a Fast fade
*/
/**************************************************************************/

 else if(animationState == 3) // Quick flutter
 {
// set enablePin high so that LEDs turn on:
    digitalWrite(enablePin, HIGH);

//turn on LED L1

  pinMode(LED_1, OUTPUT);     //row 1 is output
  digitalWrite(LED_1, LOW);  // row 1 is low
  pinMode(LED_2, OUTPUT);     //row 2 output
  digitalWrite(LED_2, HIGH);  // row 2 is high
  int x = 5;
   for (int i = 0; i > -1; i = i + x){ // Starting with i = 0 for i> -1, i increments by x which is +-5 depending on direction
      analogWrite(enablePin, i);
      analogWrite(LED_3, i);
      if (i == 255) x = -5;   
      if (i==255) delay(5); // switch direction at peak
      delay(5);
   }
  delay(10);
  
  //turn on LED L2
  
    pinMode(LED_1, OUTPUT);     //row 1 is output
  digitalWrite(LED_1, HIGH); //row 1 is high
  pinMode(LED_2, OUTPUT);     //row 2 is output
  digitalWrite(LED_2, LOW);   // row 2 is low
 int y = 5;
   for (int i = 0; i > -1; i = i + y){
      analogWrite(enablePin, i);
      analogWrite(LED_3, i);
      if (i == 255) y  = -5; 
       if (i==255) delay(5);// switch direction at peak with a 5 millisecond delay
      delay(5);
    }
  delay(10);
 }

/**************************************************************************/
/*!
    animationState == 4
    Light complete a Medium Twinkle
*/
/**************************************************************************/
  else if(animationState == 4) // Twinkle
 {
// set enablePin high so that LEDs turn on:
    digitalWrite(enablePin, HIGH);
  
//turn on LED L1

   pinMode(LED_1, OUTPUT);     //row 1 is output
  digitalWrite(LED_1, LOW);  // row 1 is low
  pinMode(LED_2, OUTPUT);     //row 2 output
  digitalWrite(LED_2, HIGH);  // row 2 is high
  int x = 15;
   for (int i = 0; i > -1; i = i + x){ // Starting with i = 0 for i> -1, i increments by x which is +-15 depending on direction ( larger increments for twinkle)
      analogWrite(enablePin, i);
      analogWrite(LED_3, i);
      if (i == 255) x = -15;   
      if (i==255) delay(20); // switch direction at peak since intervals are higher, the delay is 20 milliseconds instead of 5 from the fast fade. This gives twinkle effect.
      delay(10);
   }

//turn on LED L2

    pinMode(LED_1, OUTPUT);     //row 1 is output
  digitalWrite(LED_1, HIGH); //row 1 is high
  pinMode(LED_2, OUTPUT);     //row 2 is output
  digitalWrite(LED_2, LOW);   // row 2 is low
 int y = 15;
   for (int i = 0; i > -1; i = i + y){ // Starting with i = 0 for i> -1, i increments by y which is +-15 depending on direction ( larger increments for twinkle)
      analogWrite(enablePin, i);
      analogWrite(LED_3, i);
      if (i == 255) y  = -15; 
       if (i==255) delay(20);// switch direction at peak since intervals are higher, the delay is 20 milliseconds instead of 5 from the fast fade. This gives twinkle effect.
      delay(10);
   }
 }
}
