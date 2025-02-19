#include <AlmostRandom.h>
#include <InsertionSort.h>
#include <NewPing.h>
#include <Motor.h>

Motor rightMotor = Motor(8, 5);
Motor leftMotor = Motor(12, 6);
NewPing sonar(A2, A3);

//used for random generation
AlmostRandom ar;

#define START_BTN A0  // the Arduino pin where the start button is attached

void setup() {
  Serial.begin(115200);
  pinMode(START_BTN, INPUT_PULLUP);
  //wait for the start buton
  //WaitForButton();
  Square(4, 50);
  WaitForButton();
  Square(4, 80);
  WaitForButton();
  FigureEight();
  WaitForButton();
}

void loop() {
  ObstacleAvoidanceLoop();
}

void WaitForButton() {
  while (digitalRead(START_BTN) == HIGH)
    ;
  delay(2000);
}

void ObstacleAvoidanceLoop() {
  //we check if it is more than 30 cms away, or if the sensor reports 0
  //The sensor reports 0 if it doesnt hit anything
  int pingDist = sonar.ping_cm();
  while (pingDist >= 30 || pingDist == 0) {
    Forward(50);
    pingDist = sonar.ping_cm();  //update the ping distance value
  }

  //randomly determine if to turn left or right
  if (ar.getRandomByte() > 128) {
    Turn(90, 50);
  } else {
    Turn(-90, 50);
  }
}

//Drive in a arbitrary figure eight roughyl 4ft in diameter
void FigureEight() {
#define circleTime 15000
#define turnRatio 6
  DifferentialForward(50, turnRatio);
  delay(circleTime);
  DifferentialForward(50, -(turnRatio - 1));
  delay(circleTime);
  Stop();
}

//Drive in a square with a sidelength in ft and a specific speed percentage 0 to 100
void Square(int sideLength, int speedPct) {
#define turnSpeed 25
  Move(sideLength, speedPct);
  Turn(90, turnSpeed);
  Move(sideLength, speedPct);
  Turn(90, turnSpeed);
  Move(sideLength, speedPct);
  Turn(90, turnSpeed);
  Move(sideLength, speedPct);
  Turn(90, turnSpeed);
}

//Move a specific ammount of ft at a speed percentage 0 to 100
//supports going forwards and backwards by using a negative in the distance
void Move(int distanceUnits, int speedPct) {
//hand tuned scalar to make the distanceUnit match 1 ft
#define distanceScalar 42500
  int timeDelay = (distanceScalar * abs(distanceUnits)) * (1.0 / speedPct);
  if (distanceUnits > 0) {
    Forward(speedPct);
  } else {
    Backwards(speedPct);
  }
  delay(timeDelay);
  Stop();
}

//Turn by a specified ammount of degrees either positive or negative by a 0 to 100 pct
void Turn(int degrees, int speedPct) {
//hand tuned scalar to make the degrees match with the delay
#define degreesScalar 265
  int timeDelay = (degreesScalar * abs(degrees)) * (1.0 / speedPct);
  if (degrees > 0) {
    Right(speedPct);
  } else {
    Left(speedPct);
  }
  delay(timeDelay);
  Stop();
}

//General movement commands with a percentage
void Forward(int speedPct) {
  rightMotor.rotateCW(speedPct + 2); //adjusted to account for right wheel being weird
  leftMotor.rotateCCW(speedPct);
}

void Backwards(int speedPct) {
  rightMotor.rotateCCW(speedPct);
  leftMotor.rotateCW(speedPct);
}

void Right(int speedPct) {
  rightMotor.rotateCCW(speedPct);
  leftMotor.rotateCCW(speedPct);
}

void Left(int speedPct) {
  rightMotor.rotateCW(speedPct);
  leftMotor.rotateCW(speedPct);
}

//Drives the robot forward with a differential between both wheels
//Positive is right, negative is left
void DifferentialForward(int speedPct, int ratio) {
  rightMotor.rotateCW(speedPct - ratio);
  leftMotor.rotateCCW(speedPct + ratio);
}

//stop both motors, with a delay afterwards to prevent drifting
void Stop() {
  rightMotor.stop();
  leftMotor.stop();
  delay(500);  //wait half a second to avoid drifting
}
