/*
 Author: Sohail Shah
 Date: 18/01/2019

 Description:

 A person tracker sketch using the Ard Object Detection and Tracking Android App from Gemcode Studios.
 It uses a standard 4W robot vehicle kit connected to a motor driver (use two if more turning torque is required. 
 An Arduino with a Bluetooth HC-05 device.

 Disclaimer: The code is supplied as is and the author does not hold any responsiblity for loss or damage as a
 result of usage.
  
 */

#include <NewPing.h>
int trigPin = 7;
int echoPin = 8;
NewPing sonar(trigPin, echoPin, 200);

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


const int IN1 = 5;            // Use a standard hobby motor driver which has
const int IN2 = 6;            // two motor outputs. Combination of Inputs
const int IN3 = 10;           // control clockwise and counter clockwise rotation.
const int IN4 = 11;           // 

int turnSpeed = 0;            // How fast robot should turn. Calculated during detection. Balance with minSpeed setting.
int maxFOV = 600;             // Field of view in pixels from centre

int minSpeed = 120;           // Motors will starting turning around 120 value. Change to suit your motors.
int turnCount = 0;            // Count of how many times we have turned.
int forwardCount = 0;         // Forward count
int searchCount = 0;          // How many times have we searched.
int maxSearchWaitTime = 3000; // How long before searching people. (3 seconds)
int maxSearchCount = 10;
int maxForwardCount = 25;
int maxTurnCount = 5;
bool bGoForward = false;
bool bTurning = false;
bool bTurningRightLast = true;

int xVal = -2000;
int yVal = -2000;
String sXVal = "";            // Horizontal x value of target from centre 0,0
String sYVal = "";            // Vertical y value of target from centre 0,0
int distance = 1;             // Used to hold distance from sonar reading.

unsigned long StartTime = millis();

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

  bool bObjectFound = false;
  
  //
  // Handle Object Tracking:
  //
  if (stringComplete) {
    //Serial.flush(); // To avoid too much buffer building up whilst processing below:
    //
    // Check if filtering on specfic tensor flow objects and colour intersection. Here we are checking person objection. 
    // You could easily change object types to that which suit your application. E.g. you can change this to dog or cat
    // to track animals. 
    // To set up this in Ard Object Detection Tracking App: Select both colour blob and colour tracking, with Tensor Flow Detection and Filtering On.
    // For a full list of the object types supported by Tensor Flow Lite see Tensor Flow Lite on Google
    // site:
    //
    if (inputString.startsWith("FTF:person:"))
    {
      bObjectFound = true;
      searchCount = 0;
      
      //Serial.println(inputString);
      inputString.replace("FTF:person:", "");

      int colonPos = inputString.indexOf(":");
      if (colonPos>0)
      {
        String objID = inputString.substring(0, colonPos);
        inputString.replace(objID+":", "");

        int commaPos = inputString.indexOf(",");
        if (commaPos>0)
        {
          sXVal = inputString.substring(0, commaPos);
          xVal = sXVal.toInt();
          int commaPos2 = inputString.indexOf(",", commaPos+1);
          sYVal = inputString.substring(commaPos+1, commaPos2);
          yVal = sYVal.toInt();

          Serial.println(sXVal + ":" + sYVal);
        }
      } 
    }
    //
    // Standard tensor flow person object detected. If using this note robot may get confused between different people in close proximity:
    // From tests conducted this will however work faster than the above filtered detection. For this just select Tensor Flow Object Detection Only.
    //
    else if (inputString.startsWith("person:")) 
    {
      Serial.println(inputString);
      inputString.replace("person:", "");
      bObjectFound = true;
      searchCount = 0;
      
      int colonPos = inputString.indexOf(":");
      if (colonPos>0)
      {
        String objID = inputString.substring(0, colonPos);  // Could do something with ObjID, but ignored for now.
        inputString.replace(objID+":", "");

        int commaPos = inputString.indexOf(",");
        if (commaPos>0)
        {
          sXVal = inputString.substring(0, commaPos);
          xVal = sXVal.toInt();
          int commaPos2 = inputString.indexOf(",", commaPos+1);
          sYVal = inputString.substring(commaPos+1, commaPos2);
          yVal = sYVal.toInt();

          Serial.println(sXVal + ":" + sYVal);
        }
      }
    }
    
    // Is object detected if so now track it:
    if (bObjectFound)
    {
      searchCount = 0;    // Resets the search count so we can try search in case we lose the object during subsequent frames.
      
      xVal = constrain(xVal, -maxFOV, maxFOV);  
      
      // Handle Forward Movement:
      if (xVal > -100 && xVal < 100)
      {
        Serial.println("Go forward:");
        bGoForward = true;
        bTurning = false;
        forward(255);
        forwardCount = 0;
      }
      else if (xVal < -100)
      {
        xVal = abs(xVal);
        // Note: avoid having minSpeed set too low as motors will not move. Too high it will overshoot target.
        turnSpeed = map(xVal, 100, 600, minSpeed, 255); 
        Serial.println("Go Left:" + String(turnSpeed));
        left(turnSpeed);
        turnCount = 0;
        bTurning = true;
        bGoForward = false;  
        bTurningRightLast = false;
      }
      else if (xVal > 100)
      {
        turnSpeed = map(xVal, 100, 600, minSpeed, 255);
        Serial.println("Go right:" + String(turnSpeed));
        right(turnSpeed);
        turnCount = 0;
        bTurning = true;
        bGoForward = false;
        bTurningRightLast = true;
      }    
    }
    else 
    {
      // Object Not Found:
      StartTime = millis(); // Start wait process until max wait time has elapsed before searching.
    }
      
     
    // clear the string:
    inputString = "";
    stringComplete = false;
  } 

  // Check if turning and control how long we continue turning before stopping.
  if (bTurning && turnCount > maxTurnCount)
  {
    Serial.println("TurnCount="+String(turnCount));
    bTurning = false;
    stop(1);
    turnCount = 0;
  }
  else
  {
    turnCount++;
  }

  // Check if going forward and control how long we continue going forward before stopping.
  if (bGoForward && forwardCount > maxForwardCount)
  {
    bGoForward = false;
    stop(1);
    forwardCount = 0;
  }
  else
  {
    forwardCount++;
    delay(10);
  }


  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  if (ElapsedTime > maxSearchWaitTime && searchCount < maxSearchCount)
  {
    // Do search:
    bTurning = false;
    bGoForward = false;
    Serial.println("Searching After="+String(ElapsedTime));
    if (bTurningRightLast)
      right(255);
    else 
      left(255);  
    delay(200);
    stop(1);
    // Reset start time:
    StartTime = CurrentTime;
    searchCount++;
  }

  distance = sonar.ping_cm();
  //Serial.println(distance);
  if (distance !=0 && distance < 50)
  {
    bGoForward = false;
    reverse(255);
    delay(30);
    stop(1);        
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

void left(int speed)
{
    analogWrite(IN1, speed);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, speed);
}

void right(int speed)
{
    analogWrite(IN1, 0);
    analogWrite(IN2, speed);
    analogWrite(IN3, speed);
    analogWrite(IN4, 0);
}

void forward(int speed)
{
    analogWrite(IN1, speed);
    analogWrite(IN2, 0);
    analogWrite(IN3, speed);
    analogWrite(IN4, 0);
}

void reverse(int speed)
{
    analogWrite(IN1, 0);  
    analogWrite(IN2, speed);
    analogWrite(IN3, 0);
    analogWrite(IN4, speed);
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
