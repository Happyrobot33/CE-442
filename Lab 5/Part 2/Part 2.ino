#include <AlmostRandom.h>
#include <InsertionSort.h>
#include <NewPing.h>
#include <Motor.h>
#include <Wire.h>

Motor rightMotor = Motor(8, 5);
Motor leftMotor = Motor(12, 6);
NewPing sonar(A2, A3);

#define START_BTN A0 // the Arduino pin where the start button is attached

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    pinMode(START_BTN, INPUT_PULLUP);
    // wait for the start buton
    // WaitForButton();
}

#define SETDISTANCE 25

//propotional control
#define kP 4

void loop()
{
    int pingDist = sonar.ping_cm();
    int error = (SETDISTANCE - pingDist) * kP;

    if (pingDist != 0)
    {
        // too close
        Drive(-error);
    }
    else
    {
        Stop();
    }
}

// wait until the button on the robot is pressed to continue execution. This is blocking
void WaitForButton()
{
    while (digitalRead(START_BTN) == HIGH)
        ;
    delay(2000);
}

// Turn by a specified ammount of degrees either positive or negative by a 0 to 100 pct
void Turn(int degrees, int speedPct)
{
// hand tuned scalar to make the degrees match with the delay
#define degreesScalar 265
    int timeDelay = (degreesScalar * abs(degrees)) * (1.0 / speedPct);
    if (degrees > 0)
    {
        Right(speedPct);
    }
    else
    {
        Left(speedPct);
    }
    delay(timeDelay);
    Stop();
}

void Drive(int speedPct)
{
    if (speedPct > 0)
    {
        Forward(speedPct);
    }
    else
    {
        Backwards(abs(speedPct));
    }
}

// General movement commands with a percentage
void Forward(int speedPct)
{
    rightMotor.rotateCW(speedPct + 2); // adjusted to account for right wheel being weird
    leftMotor.rotateCCW(speedPct);
}

void Backwards(int speedPct)
{
    rightMotor.rotateCCW(speedPct);
    leftMotor.rotateCW(speedPct);
}

void Right(int speedPct)
{
    rightMotor.rotateCCW(speedPct);
    leftMotor.rotateCCW(speedPct);
}

void Left(int speedPct)
{
    rightMotor.rotateCW(speedPct);
    leftMotor.rotateCW(speedPct);
}

// Drives the robot forward with a differential between both wheels
// Positive is right, negative is left
void DifferentialForward(int speedPct, int ratio)
{
    rightMotor.rotateCW(speedPct - ratio);
    leftMotor.rotateCCW(speedPct + ratio);
}

// stop both motors, with a delay afterwards to prevent drifting
void Stop()
{
    rightMotor.stop();
    leftMotor.stop();
    delay(500); // wait half a second to avoid drifting
}
