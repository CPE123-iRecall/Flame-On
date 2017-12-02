/*

Wiring:
1.  VCC to 5V on Arduino board
2.  GND to GND on Arduino board
3.  REC to Pin 11 on Arduino board
4.  P-E to Pin 13 on Arduino board

*/

#include <CPE123_Fall17.h>

const int Rec = 53;
const int Play = 51;
const int buttonPin = 4;

//Hardware
Button ignitionButton(buttonPin);

void setup()
{ 
  pinMode(Rec, OUTPUT);
  pinMode(Play, OUTPUT);
}

void loop()
{
  playAlert();
}  

void playAlert()
{
  enum {PLAYING, NOT_PLAYING};
  static int state = NOT_PLAYING;
  static MSTimer aTimer;
  switch(state)
  {
    case NOT_PLAYING:
      if (aTimer.done())
      {
        aTimer.set(2000);
        digitalWrite(Play, HIGH);
        state = PLAYING;         
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

