#include <Wire.h>
#include <Servo.h>
const int rgbGreen = 4;
const int rgbBlue  = 5;
const int rgbRed   = 6;
int RValue = 0;
int GValue = 0;
int BValue = 0;
int actuatorType;
int state = 0;
Servo AC;

void setup()
{
  AC.attach(2);
  pinMode(rgbGreen, OUTPUT);
  pinMode(rgbBlue, OUTPUT);
  pinMode(rgbRed, OUTPUT);
  Wire.begin(0x22);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  Serial.println("start slave");
}

void loop()
{
  delay(100);
}

void receiveEvent(int howMany){
  actuatorType = Wire.read();
  Serial.print("actuator: ");
  Serial.print(actuatorType);
  Serial.print(", ");
  if (actuatorType == 0){
    state = Wire.read();
    Serial.println(state);
    if (state == 1){
      AC.write(45);
    }
    else{
      AC.write(90);
    }
  }
  else if (actuatorType = 1){
    state = Wire.read();
    Serial.println(state);
    if (state == 1){
      analogWrite(rgbGreen, 255);
      analogWrite(rgbBlue, 255);
      analogWrite(rgbRed, 255);
    }
    else{
      analogWrite(rgbGreen, 0);
      analogWrite(rgbBlue, 0);
      analogWrite(rgbRed, 0);
    }
  }
    else if (actuatorType = 2){
      RValue = Wire.read();
      GValue = Wire.read();
      BValue = Wire.read();
      analogWrite(rgbGreen, GValue);
      analogWrite(rgbBlue, BValue);
      analogWrite(rgbRed, RValue);
  }
}
