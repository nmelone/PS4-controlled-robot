/*
 * RobotCommands.iso
 *
 *  Created on: Sep 30, 2017
 *      Author: redsc_000
 */

// Including all Libraries used for RobotCommands ----------
#include <Wire.h>
#include <PS4BT.h>
#include <usbhub.h>
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>


// Pin Out --------------------------------------------------
const int pingPin = 30;
const int motorPins[4] = {3,5,6,7};
const int directionPins[4] = {22,24,26,28};


// Constant Variables ---------------------------------------
const bool DEBUG = 0; 
const int neutral = 127;
const int forward = 254;

// Changing Variables ---------------------------------------
//Robots
unsigned char irReadings[8];
int motorSpeeds[4];
unsigned long cm;
int drive = 0;
int strafe = 0;
int rotate = 0;
int armCommand = 0;//changed to int from char set to 0 instead of 10

//USB Shield

bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;

// Class Objects --------------------------------------------
USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd, PAIR);
// PS4BT PS4(&Btd);

// Intial Setup ---------------------------------------------

void setup() {
  Wire.begin(); // join I2C bus
  Serial.begin(115200);
  #if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));

  for(int i = 0; i <4; i++) {
    pinMode(motorPins[i], OUTPUT);
    pinMode(directionPins[i], OUTPUT);
  }
 
}

// Main Loop ------------------------------------------------

void loop() {
  Usb.Task(); 
  if (PS4.connected()) { //PS4 button checks will be in this if() statement

    //Motor Controls
    if (PS4.getAnalogHat(LeftHatX) > 137 || PS4.getAnalogHat(LeftHatX) < 117 
    || PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117
    || PS4.getAnalogHat(RightHatX) > 137 || PS4.getAnalogHat(RightHatX) < 117
    || PS4.getAnalogHat(RightHatY) > 137 || PS4.getAnalogHat(RightHatY) < 117) {

      driveMotorsWithPS4();
    } else {
      stopMotors();
    } //End of PS4.getAnaloghat if() statement 

    //Arm Controls
    /*CURRENT ISSUE 05-OCT-2017
     * When set to getButtonPress causes arm to go crazy
     * activates more than the intended servo cause unknown
     * when the button is release arm returns to the position it was
     * already in before pressing the button
     * FROM LIBRARY NOTES
     * getButtonPress(ButtonEnum b) will return true as long as the button is held down.
     * While getButtonClick(ButtonEnum b) will only return it once.
     */
    if(PS4.getButtonClick(TRIANGLE)|| PS4.getButtonPress(TRIANGLE)){//open the elbow
      armCommand = 0;
      tellArm();
    } //End Triangle if() statement

    if(PS4.getButtonClick(SQUARE)||PS4.getButtonPress(SQUARE)){//close the elbow
      armCommand = 1;
      tellArm();
    }

    if (PS4.getButtonClick(RIGHT)||PS4.getButtonPress(RIGHT)){//pivot arm right
      armCommand = 2;
      tellArm();
    } //End Right if() statement

    if (PS4.getButtonClick(LEFT)||PS4.getButtonPress(LEFT)){//pivot arm left
      armCommand = 3;
      tellArm();
    } //End Left if() statement

    if (PS4.getButtonClick(UP)|| PS4.getButtonPress(UP)){//move shoulder up
      armCommand = 4;
      tellArm();
    } //End Up if() statement
    
    if (PS4.getButtonClick(DOWN)||PS4.getButtonPress(DOWN)){//move shoulder down
      armCommand = 5;
      tellArm();
    }  //End Down if() statement

    if(PS4.getButtonClick(L1)){//open gripper
      armCommand = 6;
      tellArm();
    } //End L1 if() statement

    if(PS4.getButtonClick(R1)){//close gripper
      armCommand = 7;
      tellArm();
    } //End R1 if() statement

    if(PS4.getButtonClick(CIRCLE)|| PS4.getButtonPress(CIRCLE)){//raise wrist
      armCommand = 8;
      tellArm();
      }

    if(PS4.getButtonClick(CROSS)||PS4.getButtonPress(CROSS)){//move wrist down
      armCommand = 9;
      tellArm();
    } //End Cross if() statement

    if(PS4.getButtonClick(OPTIONS) ||PS4.getButtonPress(OPTIONS)){//reset arm
      armCommand = 13;
      tellArm();
    }

    if(PS4.getButtonClick(SHARE)){//debug for arm
      armCommand = 11;
      tellArm();
      askIR();
      askPing();
    }

    
    if (PS4.getButtonClick(TOUCHPAD)) {
       autoMode();
    }

    if(PS4.getButtonClick(PS)|| PS4.getButtonPress(PS)){
      armCommand = 12;
      tellArm();
    }

//    if(PS4.getButtonClick(PS)){
//      for(int j = 20; j < 45; j++){
//        armCommand = j;
//        tellArm();
//        delay(500);
//      }
//    }
    
    //Sensor Controls
    
  }//End of PS4.connected if() statement
}

// Functions ------------------------------------------------
// Sensor Functions
void askIR(void){
/*
 * Used to update IR sensor readings.
 * Sends I2C request to bus address #9. Asks for 16 bytes, only need 0,2,4,8,10,12,14
 * Pass in an char array of 8 to be modified with new sensor readings, [0-255]
 * #DEFINE irReadings
 */
  static unsigned char t, data[16];

  t = 0;

  Wire.requestFrom(9,16); // request 16 bytes from slave device #9
  while (Wire.available()) { // slave may send less than requested
    data[t] = Wire.read(); // receive a byte as character
    t++;
  }
  for(char i = 0; i < 8; i++){
    irReadings[i] = data[i*2];
  }
  
  if(DEBUG) {
    Serial.print("-----IR Readings-----\n");
    for(char i = 0; i < 8; i++){
      Serial.print(irReadings[i]);
      Serial.print("~"); 
    }
    Serial.print("\n");
  }
}

int askPing(void){
/*
 * Sends signal to activate Ping sensor and returns the distance in centimeters (may try using mm)
 * Process takes 7-8 milliseconds.
 * DEFINE pingPin
 */
  static unsigned long duration;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance traveled.
  cm = duration / 29 / 2;
  
  if(DEBUG) {
    Serial.print("-----Ping(cm)------\n");
    Serial.print(cm);
    Serial.print("\n");
  }
}

//Arm Funcitons

void tellArm(void){
/*
 * Should send the desired locations of the arm to the Arduino Uno over I2C at address #8
 * Format of the data needs to be determined. Speed,Joint,Degree in a single signal or in 6 joint
 * intervals with speeds being controlled by analog pin or something
 */

    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(armCommand);
    Wire.endTransmission(); // stop transmitting\
    
    if(DEBUG) {
      Serial.print("-----Arm Command----\n");
      Serial.print(armCommand);
      Serial.print("\n");
    }
}

//Motor Functions

void tellMotors(void){
/*
 * Uses the [(-)255-255] speed variables and sends the signals to motors.
 * DEFINE directionPins and motorPins
 */
  for(int i = 0; i < 4; i++){
    if(motorSpeeds[i] < 0) {
      digitalWrite(directionPins[i], LOW);
    }else{
      digitalWrite(directionPins[i], HIGH);
    }
    
    if(abs(motorSpeeds[i])*2 < 255){
      analogWrite(motorPins[i], abs(motorSpeeds[i])*2);
    }else{
      analogWrite(motorPins[i], 255);
    }
  }
}

void driveStraight(int drive){
/*
 * Drives motors forwards or backwards(negative) at set speed
 * Using for autonoumous part 
 */
  for(int i = 0; i < 4; i ++) {
    if(drive < 0) {
      digitalWrite(directionPins[i], LOW);
    }else{
      digitalWrite(directionPins[i], HIGH);
    }
    analogWrite(motorPins[i], abs(drive));
  }
}


void turnRight(int drive,int degree){
/*
 * Drives motors so that the right motors are slower and the robot turns
 * Degree is the percentage difference of left and right motors, Ex: If drive = 100 and degree 60, the right motors will run at 60 while left at 100
 * Using for autonoumous part 
 */
  for(int i = 0; i < 4; i ++) {
 
    digitalWrite(directionPins[i], HIGH);
    
    if(i == 1 || i == 2) {
      analogWrite(motorPins[i], abs((drive*degree)/100));
    }else{
      analogWrite(motorPins[i], abs(drive));
    }
    
  }
}

void turnLeft(int drive,int degree){
/*
 * Drives motors so that the left motors are slower and the robot turns
 * Degree is the percentage difference of left and right motors, Ex: If drive = 100 and degree 60, the right motors will run at 60 while left at 100
 * Using for autonoumous part 
 */
  for(int i = 0; i < 4; i ++) {
 
    digitalWrite(directionPins[i], HIGH);
    
    if(i == 0 || i == 3) {
      analogWrite(motorPins[i], abs((drive*degree)/100));
    }else{
      analogWrite(motorPins[i], abs(drive));
    }
    
  }
}

void stopMotors(void) {
  //stop the bot
  for(int i = 0; i < 4; i ++) {
    analogWrite(motorPins[i], 0);
  }
}

void motorMath(void) {
  //calculating drive speed and strafe speed
  drive = (forward - PS4.getAnalogHat(LeftHatY)) - neutral;
  strafe = PS4.getAnalogHat(LeftHatX) - neutral;
  rotate = PS4.getAnalogHat(RightHatX) - neutral;
  //calculating wheel speed
  motorSpeeds[1] = drive + strafe - rotate;
  motorSpeeds[0] = drive - strafe + rotate;
  motorSpeeds[2] = drive - strafe - rotate;
  motorSpeeds[3] = drive + strafe + rotate;
}

void driveMotorsWithPS4(void){
  motorMath();
  tellMotors();
}

//Autonomous Functions 

void checkIR(void){
/*
 * Updates irReadings[i] with new values and checks to see if they fit our positioning conditions
 * Flashes PS4 controller light Green when Good, Flash PS4 light Red when not 
*/
  askIR();
  if((irReadings[3] > 150) && (irReadings[4] > 150)){
    PS4.setLedFlash(10,10);
    PS4.setLed(Green);
    delay(1000);
    PS4.setLedFlash(0,0);
    PS4.setLedOff();
  } else {
    PS4.setLedFlash(10,10);
    PS4.setLed(Red);
    delay(1000);
    PS4.setLedFlash(0,0);
    PS4.setLedOff();
  }
}

void autoMode(void){
  cm = 50;
  while(cm > 6){


    askPing();
    askIR();
    if((irReadings[0] > 200) && (irReadings[1] > 200) && (irReadings[2] > 200)
      && (irReadings[3] > 200) && (irReadings[4] > 200) && (irReadings[5] > 200)
      && (irReadings[6] > 200) && (irReadings[7] > 200)){
      stopMotors();
      return;
    }

    if((irReadings[3] < 150) || (irReadings[4] < 150)){
      driveStraight(100);
      if(DEBUG) {
        Serial.print("------Straight------\n");
        for(char i = 0; i < 8; i++){
          Serial.print(irReadings[i]); 
          Serial.print("~");
        }
        Serial.print("\n");
       }
      
    } else if((irReadings[0] < 150) || (irReadings[1] < 150) || (irReadings[2] < 150)){
      turnLeft(60,20);
      if(DEBUG) {
        Serial.print("------Left------\n");
        for(char i = 0; i < 8; i++){
          Serial.print(irReadings[i]); 
          Serial.print("~");
        }
        Serial.print("\n");
       }
    } else if((irReadings[5] < 150) || (irReadings[6] < 150) || (irReadings[7] < 150)){
      turnRight(60,20);
      if(DEBUG) {
        Serial.print("------Right------\n");
        for(char i = 0; i < 8; i++){
          Serial.print(irReadings[i]); 
          Serial.print("~");
        }
        Serial.print("\n");
       }
    }
  }
  stopMotors();
  armCommand = 6; //Open Gripper
  tellArm();
}



//END-----------------------------------------------------------
/*DEBUG Temp
 *  if(DEBUG) {
    Serial.print("------------------\n");
    for(char i = 0; i < 8; i++){
      Serial.print(); 
    }
    Serial.print("\n");
    }
 */


