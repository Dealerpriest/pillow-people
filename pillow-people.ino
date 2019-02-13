#define DEBUG
#ifndef DEBUG
#define PRINT_LN(x)
#define PRINT(x)
#else
#define PRINT_LN(x) Serial.println(x)
#define PRINT(x) Serial.print(x)
#endif

// #include "touchAdvanced.c"
#include "cap-sense.h"
#include "motor-control.h"

const int resetCalibrationPin = 2;
const int motorActivationPin = 4;
int touchPin = 15;
int pwmLedPin = 23;
int pwmValue = 0;
unsigned long heartStamp;
unsigned long heartInterval = 500;

CapSense capPins[3] = {CapSense(15), CapSense(19), CapSense(18)};
const int nrOfCapPins = sizeof(capPins) / sizeof(CapSense);
MotorControl motorPins[3] = {MotorControl(9), MotorControl(10), MotorControl(6)};
const int nrOfMotorPins = sizeof(motorPins) / sizeof(MotorControl);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(resetCalibrationPin, INPUT_PULLUP);
  pinMode(motorActivationPin, INPUT_PULLUP);

  for (int i = 0; i < nrOfCapPins; ++i)
  {
    capPins[i].initialize();
  }

  Serial.begin(57600);

  digitalWrite(LED_BUILTIN, LOW);
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(50);
  }
  heartStamp = millis();

  for (int i = 0; i < nrOfMotorPins; i++)
  {
    motorPins[i].initialize();
    motorPins[i].startMotor();
    motorPins[i].deactivatePattern();
  }
}

void loop()
{
  for (int i = 0; i < nrOfMotorPins; i++)
  {
    motorPins[i].update();
  }

  for (int i = 0; i < nrOfCapPins; i++)
  {
    int capValue = capPins[i].readWithAutoCal();
    // Serial.printf("%i\t", capValue);
    // capPins[i].logDebugEvents(false);
    capPins[i].logValues(false);
    if (!digitalRead(motorActivationPin))
    {
      motorPins[i].setMotorSpeed(map(capValue, 0, 1000, 0, 255));
    }
    else
    {
      motorPins[i].setMotorSpeed(0);
    }
  }

  Serial.printf("10000\n");

  delay(10);

  if (millis() - heartStamp > heartInterval)
  {
    heartStamp = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  if (millis() < 2000 || !digitalRead(resetCalibrationPin))
  {
    resetAllCapCalibration();
  }
}

void resetAllCapCalibration()
{
  for (int i = 0; i < nrOfCapPins; i++)
  {
    capPins[i].resetMinMaxCapValues();
  }
}