#include <Wire.h>
const int echoUltrasonic = 2;
const int trigUltrasonic = 3;
const int PIR = 4;
int tempSensor = A0;
float duration = 0;
float distanceCM = 0;
int movementExist = 0;
int personPass = 0;
int personExist = 0;
float celcius = 0;
unsigned long currentTime = 0;
unsigned long startTime = 0;
int autoMode = 1;
int ACStatus = 0;
int LEDStatus = 0;
int personExistBefore = 0;
String dataPython = "";
       
void setup() {
  pinMode(trigUltrasonic, OUTPUT);
  pinMode(echoUltrasonic, INPUT);
  pinMode(PIR, INPUT);
  pinMode(tempSensor, INPUT);
  Wire.begin();
  Serial.begin(9600);
}

int checkUltrasonic(){
  int ultrasonicStatus = 0;
  digitalWrite(trigUltrasonic, LOW);
  delayMicroseconds(2);
  digitalWrite(trigUltrasonic, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigUltrasonic, LOW);
  duration = pulseIn(echoUltrasonic, HIGH);
  distanceCM = duration*0.034/2;
//  Serial.println(distanceCM);
  if (distanceCM <= 80 && distanceCM >= 20) {
    ultrasonicStatus = 1;
  }
  return ultrasonicStatus;
}
  
int checkPIR(){
  startTime = millis();
  currentTime = millis();
  while ((currentTime - startTime)<= 2000){
    movementExist = digitalRead(PIR);
    if (movementExist == HIGH){
      return 1;
    }
    currentTime = millis();
  }
  return 0;
}

float getTemp(){
  float reading = analogRead(tempSensor);
  float voltage = reading * (3.3 / 1024.0);
  float roomTemperature = ((voltage - 0.5) * 100);
  return roomTemperature;
}

int printAllStatus(){
  Serial.print(personExist);
  Serial.print(",");
  Serial.print(getTemp());
  Serial.print(",");
  Serial.print(ACStatus);
  Serial.print(",");
  Serial.print(LEDStatus);
  Serial.print("\n");
}

int actuatorAC(int state){
  if (ACStatus != state){
    Wire.beginTransmission(0x22);
    Wire.write(0);
    Wire.write(state);
    Wire.endTransmission();
    ACStatus = state;
    printAllStatus();
  }
}

int actuatorLED(int state){
  if (LEDStatus != state){
    Wire.beginTransmission(0x22);
    Wire.write(1);
    Wire.write(state);
    Wire.endTransmission();
    LEDStatus = state;
    printAllStatus();
  }
}

//int actuatorRGB(String string){
//  Wire.beginTransmission(0x22);
//  Wire.write(1);
//  String value = "";
//  for (int i = 0; i < string.length(); i++) {
//    if (i != ","){
//      value += i;
//    }
//    else{
//      Wire.write(value);
//      value = "";
//    }
//  }
//  Wire.endTransmission();
//}

int checkTemp(int roomTemperature){
  if (roomTemperature >= 28){
    actuatorAC(1);
  }
  else{
    actuatorAC(0);
  }
}

int dataChecker(String string){
//  Serial.println(string);
  int command = string[0];
  Serial.print("tipe aktuator");
  Serial.println(command);
  if (command == 0){
    Serial.println(string[2]);
    actuatorAC(string[2]);
    //atur ac
  }
  else if (command == 1){
    Serial.println(string[2]);
    actuatorLED(string[2]);
    //atur lampu
  }
  else if (command == 2){
//    actuatorRGB(string);
    //atur rgb
  }
  else if (command == 3){
    Serial.println(string[2]);
    int autoMode = string[2];
    //atur mode
  }
}

int receiveData(){
    if (Serial.available()>0) { // check if data is available to read
      String dataPython = Serial.readString();
      dataChecker(dataPython);
    }
}

void loop(){
  receiveData();
  if (autoMode == 1){
    personPass = checkUltrasonic();
    if (personPass == 1) {
      personExistBefore = personExist;
      personExist = checkPIR();
      if (personExistBefore != personExist){
        printAllStatus();
      }
    }
    if (personExist == 1){
      //Serial.println("Person detected");
      actuatorLED(1);
      celcius = getTemp();
      checkTemp(celcius);
    }
    else{
      actuatorLED(0);
      actuatorAC(0);
    }
  }
}
