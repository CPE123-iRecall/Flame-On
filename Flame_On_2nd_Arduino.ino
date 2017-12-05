// Sets up the ESP as a station and then has the ESP connect
// to a telnet server.
//
//  Garrett Lew - Dec 2 2017

#include "CPE123_Fall17.h"
#include "CPE123_EspAPI_Fall17.h"

// Pins - also assumes using Serial1 to talk with ESP board
//const int espResetPin = 7;
//const int boardLedPin = 42;
//const int connectedLedPin = 40;
//const int tcpLedPin = 38;



const int Rec = 9;
const int Play = 5;

const int ledPin44 = 44;


const int ledPin12 = 42;
const int ledPin11 = 40;

// ESP8266 Leds
//Led boardLed(boardLedPin);
//Led connectedLed(connectedLedPin);
//Led tcpLed(tcpLedPin);

//// WiFi and TCP information 
//const char ssid[] = "FlameOn";
//const char wifPassword[] = "fourwordsalluppercase";
//const char tcpHost[] = "192.168.88.200";
//const unsigned int tcpPort = 23; // telnet port

// Used to communicate between the Mega and the ESP device
//HardwareSerial & espSerial = Serial3;

Led bluLed(ledPin12);
Led redLed(ledPin11);

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
  setupMessage(__FILE__, "Esp AP mode with Telnet server - Mega code");
  delay(500);
  
  // setup esp wifi as station and connect to a telnet server
  //setupEsp_wifiStation_telnetClient();

  pinMode(Rec, OUTPUT);
  pinMode(Play, OUTPUT);

  bluLed.off();
  redLed.off();
    
}

void loop()
{
  alarmControl();
  //playAlert(false);
  //getValue();
}

void alarmControl()
{
  Serial.setTimeout(10);
  int shouldChangeState = 0;
  enum {WAITING, ALARMING};
  static int state = WAITING;

  shouldChangeState = getValue();

  switch (state)
  {
    case WAITING:
      if (shouldChangeState > 0)
      {
        Serial.println("foo");
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

int getValue()
{
  Serial.setTimeout(10);
  int value = 0;
  if (Serial3.available() > 0)
  {
    value = Serial3.parseInt();
    //Serial.println(value);
  }
  return value;
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

//void setupEsp_wifiStation_telnetClient()
//{
//  // connects to the wifi network and then connects to the telnet server
//  
//  espSerial.begin(9600);
//  
//  setupEspLeds(boardLed, connectedLed, tcpLed, espResetPin);
//  espBoardReset("Rebooting ESP board via reset pin"); 
//    
//  // Output ESP Firmware version (helps us verify the boards is working)
//  reportEspFirmwareVersion();
//    
//  // Turn off Access Point mode on the ESP board (just incase it was turned on)
//  wifiApModeOff();
//  
//  // Connect to WiFi
//  // Note - The Esp hardware automatically tries to reconnect when it boots
//  setupAndReportWifi(ssid, wifPassword);
//  
//  // Connect to the TCP server (if port = 23 then telnet)
//  tcpClientSetupAndReport(tcpHost, tcpPort);
//
//
//}
