/*

Code for ISD1820 Voice Recording Module
by eLab Peers (C) 2014.

Visit us at:
http://www.elabpeers.com

All rights reserved.

Wiring:
1.  VCC to 5V on Arduino board
2.  GND to GND on Arduino board
3.  REC to Pin 11 on Arduino board
4.  P-E to Pin 13 on Arduino board

*/

const int Rec = 53;
const int Play = 51;
const int buttonPin = 4;

void setup()
{ 
  pinMode(Rec, OUTPUT);
  pinMode(Play, OUTPUT);
}

void loop()
{
  digitalWrite(Rec, HIGH);
  delay(10000);
  digitalWrite(Rec, LOW);
  delay(5000);
  digitalWrite(Play, HIGH);
  delay(2000);
  digitalWrite(Play, LOW);
  delay(1000);
}  
