/*
 * iRecall inc. 
 * Device: Flame On
 * Created and Developed by Colton, Aaron, Jett, Sekaye, and Garrett Lew
 * Team 3
 * CPE 123 Section 13
 * Professor Hugh Smith
 * 11/30/17
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

//Hardware
Led ledOne(ledPin44);

Button overideButton(buttonPin);

Led bluLed(ledPin46);
Led redLed(ledPin48);

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
  while (Serial.available() <= 0)
  {
    //Wait
  }

  value = Serial.parseInt();

  return value;
}

void alarmControl()
{
  Serial.setTimeout(10);
  int shouldAlarm = 0;
  enum {WAITING, ALARMING};
  static int state = ALARMING;

  shouldAlarm = getValue();

  switch (state)
  {
    case WAITING:
      if (shouldAlarm > 0)
      {
        state = ALARMING;
      }
    break;

    case ALARMING:
      if (shouldAlarm <= 0)
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


