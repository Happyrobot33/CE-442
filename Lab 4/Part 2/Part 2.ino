#include <AlmostRandom.h>
#include <InsertionSort.h>
#include <NewPing.h>
#include <Motor.h>
#include <Wire.h>

Motor rightMotor = Motor(8, 5);
Motor leftMotor = Motor(12, 6);
NewPing sonar(A2, A3);

// used for random generation
AlmostRandom ar;

// array for storing line sensor readings
int line_sensor[5];

#define START_BTN A0 // the Arduino pin where the start button is attached

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    pinMode(START_BTN, INPUT_PULLUP);
    // wait for the start buton
    // WaitForButton();
}

void loop()
{
    LineFollowerLoop();
    // ObstacleAvoidanceLoop();
}

// wait until the button on the robot is pressed to continue execution. This is blocking
void WaitForButton()
{
    while (digitalRead(START_BTN) == HIGH)
        ;
    delay(2000);
}

// what was the last line sensor that we saw? Defaults to 0 which is in the middle.
int lastSeenSensor = 0;
void LineFollowerLoop()
{
    // check if there is something infront of us
    int pingDist = sonar.ping_cm();
    while (pingDist < 20 && pingDist != 0)
    {
        // if there is something infront of us, stop and wait until there isnt
        Stop();
        pingDist = sonar.ping_cm();
        // we wont exit this loop until the while condition is false
    }

    // read the line sensors into the global array
    readLineSensors();
// check if the line sensors went above threshold at all. if they did, rotate 180 degrees
#define whiteThreshold 6
    // scan left to right to figure out what sensor it was
    for (int i = 0; i < 5; i++)
    {
        if (line_sensor[i] > whiteThreshold)
        {
            // apply a offset so we get negative to one side, positive to the other, and 0 in the middle
            /*
            Real sensor index
             0  1  2  3  4
            -2 -1  0  1  2
            value saved into the last seen sensor variable
            */
            lastSeenSensor = i - 2;
            // break to avoid extra computation we dont need to do
            break;
        }
    }

    Serial.println(lastSeenSensor);

    // differentially drive forward with the sensor value as the differential ratio
    DifferentialForward(40, (lastSeenSensor * 3));
}

void ObstacleAvoidanceLoop()
{
    // we check if it is more than 30 cms away, or if the sensor reports 0
    // The sensor reports 0 if it doesnt hit anything
    int pingDist = sonar.ping_cm();
    while (pingDist >= 30 || pingDist == 0)
    {
        Forward(50);
        pingDist = sonar.ping_cm(); // update the ping distance value

        readLineSensors();
        Serial.println(line_sensor[0]);
        // scan left to right to figure out what sensor it was
        bool foundBlack = false;
        for (int i = 0; i < 5; i++)
        {
#define blackThreshold 2
            if (line_sensor[i] < blackThreshold)
            {
                // apply a offset so we get negative to one side, positive to the other, and 0 in the middle
                /*
                Real sensor index
                 0  1  2  3  4
                -2 -1  0  1  2
                value saved into the last seen sensor variable
                */
                lastSeenSensor = i - 2;
                foundBlack = true;
                // break to avoid extra computation we dont need to do
                break;
            }
        }

        if (foundBlack)
        {
            if (lastSeenSensor < 0)
            {
                Move(-1, 50);
                Turn(90, 50);
            }
            else
            {
                Move(-1, 50);
                Turn(-90, 50);
            }
        }
    }

    // randomly determine if to turn left or right
    if (ar.getRandomByte() > 128)
    {
        Turn(90, 50);
    }
    else
    {
        Turn(-90, 50);
    }
}

// function for reading the 5 line sensors
void readLineSensors()
{
    int count = 0;
    int high_byte, low_byte;
    // request 10 bytes from I2C device address 0x11
    Wire.requestFrom(0x11, 10);
    int raw_result[10];
    while (Wire.available())
    {
        raw_result[count] = Wire.read();
        count += 1;
    }
    // combine the low & high bytes
    for (int i = 0; i < 5; i++)
    {
        high_byte = raw_result[i * 2] << 8;
        low_byte = raw_result[i * 2 + 1];
        line_sensor[i] = high_byte + low_byte;
    }
}

// Move a specific ammount of ft at a speed percentage 0 to 100
// supports going forwards and backwards by using a negative in the distance
void Move(int distanceUnits, int speedPct)
{
// hand tuned scalar to make the distanceUnit match 1 ft
#define distanceScalar 42500
    int timeDelay = (distanceScalar * abs(distanceUnits)) * (1.0 / speedPct);
    if (distanceUnits > 0)
    {
        Forward(speedPct);
    }
    else
    {
        Backwards(speedPct);
    }
    delay(timeDelay);
    Stop();
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
