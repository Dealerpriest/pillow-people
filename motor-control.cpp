#include "Arduino.h"
#include "motor-control.h"

MotorControl::MotorControl(int pin) : _pin(pin)
{
  // pinMode(pin, OUTPUT);
}

void MotorControl::initialize()
{
  pinMode(_pin, OUTPUT);
}

void MotorControl::setInterval(float interval)
{
  _frequency = 1.0f / interval;
}

void MotorControl::setFrequency(float frequency)
{
  _frequency = frequency;
}

void MotorControl::setMotorSpeed(int speed)
{
  _speed = speed;
  // should we deactivate pattern here
}

void MotorControl::activatePattern()
{
  _runPattern = true;
}

void MotorControl::deactivatePattern()
{
  _runPattern = false;
}

void MotorControl::setPatternMaxValue(float maxValue)
{
  _patternMaxValue = maxValue;
}

void MotorControl::setPatternMinValue(float minValue)
{
  _patternMinValue = minValue;
}

void MotorControl::startMotor()
{
  _runMotor = true;
}

void MotorControl::stopMotor()
{
  _runMotor = false;
}

void MotorControl::update()
{
  if (_runMotor)
  {
    if (_runPattern)
    {
      //TODO: convert interval to factor for sin function.
      float now = millis() / 1000.0f;
      float offset = -(2 / PI);
      float normalized = (sin((TWO_PI * _frequency * now) + offset) + 1.0f) * 0.5;
      _speed = MAXMOTORSPEED * (normalized * (_patternMaxValue - _patternMinValue) + _patternMinValue);
    }
    else
    {
    }
    analogWrite(_pin, _speed);
  }
  else
  {
    analogWrite(_pin, 0);
  }
}