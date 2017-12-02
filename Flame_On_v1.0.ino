/*
 * iRecall inc. 
 * Device: Flame On
 * Created and Developed by Colton, Aaron, Jett, Sekaye, and Garrett Lew
 * Team 3
 * CPE 123 Section 13
 * Professor Hugh Smith
 * 11/30/17
 */

/*
Wiring:
1.VCC to 5V on Arduino board
2.  GND to GND on Arduino board
3.  REC to Pin 11 on Arduino board
4.  P-E to Pin 13 on Arduino board
*/

#include <CPE123_Fall17.h>
#include "CPE123_EspAPI_Fall17.h"

//Define Pins
const int flameSensorPin = 7;

const int Rec = 53;
const int Play = 51;

const int buttonPin = 4;

const int ledPin44 = 44;

const int ledPin46 = 46;
const int ledPin48 = 48;

// Pins - also assumes using Serial1 to talk with ESP board
const int espResetPin = 53;
const int boardLedPin = 42;
const int connectedLedPin = 40;
const int tcpLedPin = 38;

//Hardware
Led ledOne(ledPin44);

Button overideButton(buttonPin);

Led bluLed(ledPin46);
Led redLed(ledPin48);

Led boardLed(boardLedPin);
Led connectedLed(connectedLedPin);
Led tcpLed(tcpLedPin);

// Used to setup the ESP as an Access Point.
const char apSSID[] = "FlameOn";
const char apWifiPassword[] = "fourwordsalluppercase";

// Used to setup the ESP as a telnet server (port 23)
const unsigned int tcpServerPort = 23; // telnet

// Used to communicate between the Mega and the ESP device
HardwareSerial & espSerial = Serial3;

void setup() 
{
  // Setup serial monitor
  Serial.begin(9600);
  setupMessage(__FILE__, "Flame On Version 1.0");

  pinMode(flameSensorPin, INPUT);

  pinMode(Rec, OUTPUT);
  pinMode(Play, OUTPUT);

  bluLed.off();
  redLed.off();

  // setup esp, AP and telnet server
  setupEsp_AP_TelnetServer();
  
  //  Put esp into serial mode (mega send/recv over network use espSerial)
  setupSerialTextMode();
  
  // just to make things faster - makes waiting on esp/Serial monitor input faster
  espSerial.setTimeout(500);
  Serial.setTimeout(10);
}

void loop() 
{
   
}

void control()
{
  enum {CHECKING, ALARMING};
  static int state = CHECKING;
  
  const int intervalTime = 10000;
  static MSTimer intervalTimer(intervalTime);

  switch(state)
  {
    case CHECKING:
      if (checkForMovement() == true)
      {
        intervalTimer.set(intervalTime);
      }
      if (checkStoveOn() && intervalTimer.done())
      {
        state = ALARMING;
      }
    break;

    case ALARMING:
      if (overideButton.wasPushed() || checkForMovement())
      {
        playAlert(false);
        blinkLeds(false);
        state = CHECKING;
      } else
      {
        playAlert(true);
        blinkLeds(true);
      }
    break;

    default:
    break;
  }
}

bool checkStoveOn()
{
  bool returnValue = false;
  if (checkFlameOn() || checkGasOn())
  {
    returnValue = true;
  }
  return returnValue;
}

bool checkFlameOn()
{
  bool returnValue = false;

  int flameSensorValue = HIGH;

  flameSensorValue = digitalRead(flameSensorPin);

  if (flameSensorValue == LOW)
  {
    returnValue = true;
  }

  return returnValue;
}

bool checkGasOn()
{
  //Place holder
  return false;
}

bool checkForMovement()
{
  bool returnValue = false;

  //Place holder
  //If movement, return true

  return returnValue;
}

void playAlert(bool shouldPlay)
{
  enum {PLAYING, NOT_PLAYING};
  static int state = NOT_PLAYING;
  static MSTimer aTimer;
  switch(state)
  {
    case NOT_PLAYING:
      if (shouldPlay)
      {
        if (aTimer.done())
        {
          aTimer.set(2000);
          digitalWrite(Play, HIGH);
          state = PLAYING;         
        }        
      }
    break;

    case PLAYING:
      if (aTimer.done())
      {
        aTimer.set(1000);
        digitalWrite(Play, LOW);
        state = NOT_PLAYING;
      }
    break;

    default:
      Serial.println("Alert Error");
    break;
  }
}

void blinkLeds(bool shouldBlink)
{
  enum {WAITING, BLU_ON_RED_OFF, BLU_OFF_RED_ON};
  static int state = WAITING;
  static MSTimer ledTimer;

  if (state == WAITING)
  {  
    if (shouldBlink)
    {
      ledTimer.set(500);
      bluLed.on();
      redLed.off();
      state = BLU_ON_RED_OFF;
    }    
  } else if (state == BLU_ON_RED_OFF)
  {
    if (shouldBlink == false)
    {
      bluLed.off();
      redLed.off();
      state = WAITING;
    }
    else if (ledTimer.done()) 
    {
      bluLed.off();
      redLed.on();
      ledTimer.set(500);
      state = BLU_OFF_RED_ON;
    } 
  } else if (state == BLU_OFF_RED_ON)
  {
    if (shouldBlink == false)
    {
      bluLed.off();
      redLed.off();
      state = WAITING;
    }
    else if (ledTimer.done())
    {
      bluLed.on();
      redLed.off();
      ledTimer.set(500);
      state = BLU_ON_RED_OFF;    
    }
  }
}

int getValue()
{
  Serial.setTimeout(10);
  int value = 0;
  if (espSerial.available() > 0)
  {
    value = espSerial.parseInt();
  }

  return value;
}

void alarmControl()
{
  Serial.setTimeout(10);
  int shouldChangeState = 0;
  enum {WAITING, ALARMING};
  static int state = ALARMING;

  shouldChangeState = getValue();

  switch (state)
  {
    case WAITING:
      if (shouldChangeState > 0)
      {
        state = ALARMING;
      }
    break;

    case ALARMING:
      if (shouldChangeState > 0)
      {
        playAlert(false);
        blinkLeds(false);
        state = WAITING;
      } else 
      {
        playAlert(true);
        blinkLeds(true);
      }
    break;

    default:
      Serial.println("error");
    break;
  }
}

void setupEsp_AP_TelnetServer()
{
  espSerial.begin(9600);
  
  // configure the Esp board
  setupEspLeds(boardLed, connectedLed, tcpLed, espResetPin);
  espBoardReset("Rebooting ESP board via reset pin"); 
    
  // Output ESP Firmware version (helps us verify the boards is working)
  reportEspFirmwareVersion();
    
  // Set up Esp in Access Point mode 
  setupWifiApMode(apSSID, apWifiPassword);
  print4("WiFi AP Mode setup complete SSID: ", apSSID, " Password: ", apWifiPassword);
  
  // Now setup the TCP server and wait (block) for a client
  tcpServerSetup(tcpServerPort, "Prof. Smith's Access Point/Telnet test");
  tcpServerWaitForClient();
  
}


