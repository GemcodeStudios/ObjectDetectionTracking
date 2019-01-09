/*
 Author: Sohail Shah
 Date: 16/12/2018

 Description:

 A simple robot following sketch using the Ard Object Detection and Tracking Android App from Gemcode Studios.
 It uses a standard 2W robot vehicle kit connected to a motor driver. An Arduino with a Bluetooth HC-05 device.

 Note: In order to obtain smooth tracking the algorithm should be based on PID control. However to keep the 
 code simple the tracking is based on just an error value (P) from centre of camera view. 

 Disclaimer: The code is supplied as is and the author does not hold any responsiblity for loss or damage as a
 result of usage.
  
 */

#include <NewPing.h>
int trigPin = 4;
int echoPin = 5;
NewPing sonar(trigPin, echoPin, 200);

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


const int IN1 = 8;  //   
const int IN2 = 11;  //  
const int IN3 = 9; //   
const int IN4 = 10; // 

float duration, cm;
int maxRange = 200;

int stepRate = 10;  // How fast robot should go forward.
int turnRate = 10;
int angleRate = 30; // How fast robot should turn.

int xVal = -2000;
int yVal = -2000;
String sXVal = "";
String sYVal = "";

bool bObjectFound = false;
bool bTurningLeftLast = false;
bool bTurningRightLast = false;
int distance = 1;

int iErrorPixels = 0;

void setup() {

  Serial.begin(9600);
  pinMode(IN1, OUTPUT); // set pins to output
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("Ard Object Detection & Tracking - Copyright: 2019");
}

void loop() {
  //
  // Handle Object Tracking:
  //
  if (stringComplete) {
    
    if (inputString.startsWith("FC:0:") || inputString.startsWith("FP:0:") || inputString.startsWith("TO:0:"))
    {
      Serial.println(inputString);
      
      if (inputString.startsWith("FC:0:"))
        inputString.replace("FC:0:", "");
      else if (inputString.startsWith("FP:0:"))
       inputString.replace("FP:0:", "");
      else if (inputString.startsWith("TO:0:"))
       inputString.replace("TO:0:", "");
       
      int commaPos = inputString.indexOf(",");
      if (commaPos>0)
      {
        sXVal = inputString.substring(0, commaPos);
        xVal = sXVal.toInt();
        int commaPos2 = inputString.indexOf(",", commaPos+1);
        sYVal = inputString.substring(commaPos+1, commaPos2);
        yVal = sYVal.toInt();
      } 

      // Is the object detected within a reasonable field of view?
      if (xVal > -600 && xVal < 600)
      {
        bObjectFound = true;
        if ( xVal >-100 && xVal < 100)
        {
          // Handle Forward Movement:
          distance = sonar.ping_cm();
          if (distance == 0 || distance > 30)
          {
            forward(stepRate*6);
            //stop(1);
            left(3); // Adjustment hack to stop veering right!
            stop(1);
                        
            bTurningLeftLast = false;
            bTurningRightLast = false;
          }
          else
          {
            stop(1);
          }
        }
        // Object to left of camera centre:
        else if (xVal < -100)
        {
          iErrorPixels = -xVal/angleRate;
          left(iErrorPixels);
          bTurningLeftLast = true;
          bTurningRightLast = false;
          stop(1);  
        }
        else if (xVal > 100)
        {
          iErrorPixels = xVal/angleRate;
          right(iErrorPixels);
          bTurningLeftLast = false;
          bTurningRightLast = true;
          stop(1);
        }
      }
      else
      {
        // Do search mode:
        if (bTurningLeftLast)
          left(angleRate);
        else if (bTurningRightLast)
          right(angleRate);
          
        stop(10);
        bObjectFound = false;
      }

    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  } 

  distance = sonar.ping_cm();
  //Serial.println(distance);
  if (distance !=0 && distance < 30)
  {  
    reverse(stepRate);
    stop(1);        
  }
  else if (distance > 30 && distance < 40)
  {
    stop(100);
  }
}

void stop(int period)
{
  //Serial.println("stop");
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 0);
  delay(period);
}

void reverse(int period)
{
  //Serial.println("forward");
  for (int i=100; i<255; i+= 10)
  {
    analogWrite(IN1, 0);
    analogWrite(IN2, i);
    analogWrite(IN3, i);
    analogWrite(IN4, 0);
    //delay(1);
  }
  delay(period);
}

void right(int period)
{
  //Serial.println("left");
  for (int i=100; i< 255; i+=50)
  {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, i);
    analogWrite(IN4, i);
    //delay(1);
  }
  delay(period);
}

void left(int period)
{
  //Serial.println("right");
  for (int i=100; i<255; i+= 10)
  {
    analogWrite(IN1, i);
    analogWrite(IN2, i);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
    //delay(1);
  }
  delay(period);
}

void forward(int period)
{
  //Serial.println("reverse");
  for (int i=100; i<255; i+= 10)
  {
    analogWrite(IN1, i);  // Slow right wheel to stop turning right!
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, i);
    //delay(1);
  }
  delay(period);
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
