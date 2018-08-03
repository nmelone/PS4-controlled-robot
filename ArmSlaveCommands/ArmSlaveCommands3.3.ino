/*
 * ArmSlaveCommands.iso
 *
 *  Created on: Sep 30, 2017
 *      Author: redsc_000
 */

// Including all Libraries used for ArmSlaveCommands ----------
#include <Wire.h>
#include <Servo.h>

// Pin Out --------------------------------------------------
const int rotatePin = 11;
const int sholderPin = 10;
const int elbowPin = 9;
const int wristPin = 5;
const int wrist_rotatePin = 6;
const int gripPin = 3;

// Constant Variables ---------------------------------------
const bool DEBUG = 0;

// Changing Variables ---------------------------------------
int rot = 0;
int pos1 = 0;
int pos2 = 0;
int x = 10; 

// Class Objects --------------------------------------------
Servo servo_rotate;
Servo servo_shoulder;
Servo servo_elbow;
Servo servo_wrist;
Servo servo_wrist_rotate;
Servo servo_grip;

// Intial Setup ---------------------------------------------
void setup() {
  Wire.begin(8); // join i2c bus with address #8

  //startUpArm();
  servo_shoulder.attach(sholderPin);
  servo_rotate.attach(rotatePin);
  servo_elbow.attach(elbowPin);
  servo_wrist.attach(wristPin);
  servo_wrist_rotate.attach(wrist_rotatePin); 
  servo_grip.attach(gripPin);

  //set arm to start position
  resetArm();
  

  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);
}

// Main Loop ------------------------------------------------
void loop() {

  //delay(100);
}

// Functions ------------------------------------------------
void receiveEvent(int howMany) { // takes in the number of bytes from master 
  /*
  * This will take in the arm commands and run the servos
  * Function runs whenever Uno receives a transmission
  */
  
  //changed from while loop to if statement might need to change for press vs click
  if (Wire.available()) { // loop through all
  
   x = Wire.read();    // receive byte as an integer
   moveArm();
   if(DEBUG) {
    Serial.print("------Arm Command-----\n");
    Serial.print(x); 
    Serial.print("\n");
    }
  }
}
  
void moveArm(void){
  
  if (x == 0){
      //TRIANGLE
      //open the elbow
      pos2 = servo_elbow.readMicroseconds();
      pos2++;
      servo_elbow.writeMicroseconds(pos2);
      
    }

    if (x == 1){
      //SQUARE
      //close the elbow
      pos2 = servo_elbow.readMicroseconds();
      pos2--;
      servo_elbow.writeMicroseconds(pos2);
      
    }

    if (x == 2){
      //RIGHT
      //pivot arm right
      rot = servo_rotate.readMicroseconds();
      rot--;
      servo_rotate.writeMicroseconds(rot);
      
    }

    if (x == 3){
      //LEFT
      //pivot arm left
      rot = servo_rotate.readMicroseconds();
      rot++;
      servo_rotate.writeMicroseconds(rot);
      
    }

    if(x == 4){
      //UP
      //move shoulder up
      pos1 = servo_shoulder.readMicroseconds();
      pos1--;
      servo_shoulder.writeMicroseconds(pos1);
      
    }

    if(x == 5){
      //DOWN
      //move shoulder down
      pos1 = servo_shoulder.readMicroseconds();
      pos1++;
      servo_shoulder.writeMicroseconds(pos1);
      
    }

    if(x == 6){
      //L1
      //open gripper
      servo_grip.writeMicroseconds(1100);
    }

    if(x == 7){
      //R1
      //close gripper
      servo_grip.writeMicroseconds(1650); //Was 1550
    }
    if(x == 8){
      //CIRCLE
      //move wrist up
      pos1 = servo_wrist.readMicroseconds();
      pos1++;
      servo_wrist.writeMicroseconds(pos1);
    }
    if( x == 9){
      //CROSS
      //move wrist down
      pos1 = servo_wrist.readMicroseconds();
      pos1--;
      servo_wrist.writeMicroseconds(pos1);
    }

    if (x == 10){
      //OPTIONS
      //reset to initial position
      resetArm();
    }

    if(x==11){
      //SHARE
      //print current positions in microseconds
      Serial.print("---------------------------\n");
      Serial.print("Rotate: ");
      Serial.print(servo_rotate.readMicroseconds());
      Serial.print("\n");

      Serial.print("Shoulder: ");
      Serial.print(servo_shoulder.readMicroseconds());
      Serial.print("\n");

      Serial.print("Elbow: ");
      Serial.print(servo_elbow.readMicroseconds());
      Serial.print("\n");

      Serial.print("Wrist: ");
      Serial.print(servo_wrist.readMicroseconds());
      Serial.print("\n");

      Serial.print("Write Rotate: ");
      Serial.print(servo_wrist_rotate.readMicroseconds());
      Serial.print("\n");

      Serial.print("Grip: ");
      Serial.print(servo_grip.readMicroseconds());
      Serial.print("\n");
    }

    // Streatched out arm for things
    if(x == 12){
      pos1 = servo_shoulder.readMicroseconds();
      if(pos1 < 1800){
        pos1++;
        servo_shoulder.writeMicroseconds(pos1);
      }else if(pos1 > 1800){
        pos1--;
        servo_shoulder.writeMicroseconds(pos1);
      }

      pos1 = servo_elbow.readMicroseconds();
      if(pos1 < 1879){
        pos1++;
        servo_elbow.writeMicroseconds(pos1);
      }else if(pos1 > 1879){
        pos1--;
        servo_elbow.writeMicroseconds(pos1);
      }

      pos1 = servo_wrist.readMicroseconds();
      if(pos1 < 1434){
        pos1++;
        servo_wrist.writeMicroseconds(pos1);
      }else if(pos1 > 1434){
        pos1--;
        servo_wrist.writeMicroseconds(pos1);
      }

      pos1 = servo_rotate.readMicroseconds();
      if(pos1 < 2245){
        pos1++;
        servo_rotate.writeMicroseconds(pos1);
      }else if(pos1 > 2245){
        pos1--;
        servo_rotate.writeMicroseconds(pos1);
      }
      
    }

     // Autonoumous arm position for dropping hacky sack
     if(x == 13){
      pos1 = servo_shoulder.readMicroseconds();
      if(pos1 < 1000){
        pos1++;
        servo_shoulder.writeMicroseconds(pos1);
      }else if(pos1 > 1000){
        pos1--;
        servo_shoulder.writeMicroseconds(pos1);
      }

      pos1 = servo_elbow.readMicroseconds();
      if(pos1 < 544){
        pos1++;
        servo_elbow.writeMicroseconds(pos1);
      }else if(pos1 > 544){
        pos1--;
        servo_elbow.writeMicroseconds(pos1);
      }

      pos1 = servo_wrist.readMicroseconds();
      if(pos1 < 1260){
        pos1++;
        servo_wrist.writeMicroseconds(pos1);
      }else if(pos1 > 1260){
        pos1--;
        servo_wrist.writeMicroseconds(pos1);
      }

      pos1 = servo_rotate.readMicroseconds();
      if(pos1 < 2238){
        pos1++;
        servo_rotate.writeMicroseconds(pos1);
      }else if(pos1 > 2238){
        pos1--;
        servo_rotate.writeMicroseconds(pos1);
      }
      
    }

      if(x == 20){
        servo_shoulder.writeMicroseconds(1500);
      }

      if(x == 21){
        servo_elbow.writeMicroseconds(1879);
      }

      if(x == 22){
        servo_wrist.writeMicroseconds(1434);
      }

      if(x == 23){
        servo_rotate.writeMicroseconds(2245);
      }

      if(x >= 24){
        int k = servo_shoulder.readMicroseconds();
        if(k < 2245){
          servo_shoulder.writeMicroseconds(k+100);
        }
      }
}

void resetArm(void){
  //writing initial position
  servo_rotate.writeMicroseconds(2238);
  //544 straight up
  servo_shoulder.writeMicroseconds(1184);
  servo_elbow.writeMicroseconds(544);
  servo_wrist.writeMicroseconds(1260);
  servo_wrist_rotate.writeMicroseconds(1500);
  servo_grip.writeMicroseconds(1100);
}

//void startUpArm(void){
//
//  servo_shoulder.attach(sholderPin);
//  for(int i = 0; i < 1800; i++) {
//  servo_shoulder.writeMicroseconds(1800);
//  delay(700);
//  }
//  
//  servo_elbow.attach(elbowPin);
//  servo_elbow.writeMicroseconds(1500);
//  delay(700);
//  
//  servo_rotate.attach(rotatePin);
//  servo_rotate.writeMicroseconds(2400);
//  delay(700);
//
//  servo_wrist.attach(wristPin);
//  servo_wrist.writeMicroseconds(800);
//
//  servo_wrist_rotate.attach(wrist_rotatePin); 
//  servo_wrist_rotate.writeMicroseconds(1500);
//  
//  servo_grip.attach(gripPin);
//  servo_grip.writeMicroseconds(1550);
//
//  servo_elbow.writeMicroseconds(1200);
//}

