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
  // should we deactivate pattern here?
  // let's do!
  deactivatePattern();
}

void MotorControl::activatePattern()
{
  _runPattern = true;
}

void MotorControl::deactivatePattern()
{
  _runPattern = false;
}

// between 0.0 & 1.0 i.e percent
void MotorControl::setPatternMaxValue(float maxValue)
{
  _patternMaxValue = maxValue;
}

// between 0.0 & 1.0 i.e percent
void MotorControl::setPatternMinValue(float minValue)
{
  _patternMinValue = minValue;
}

void MotorControl::turnOn()
{
  _runMotor = true;
}

void MotorControl::turnOff()
{
  _runMotor = false;
}

void MotorControl::update()
{
  if (_runMotor)
  {
    if (_runPattern)
    {
      float now = millis() / 1000.0f;
      float offset = -(2 / PI);
      float normalized = (sin((TWO_PI * _frequency * now) + offset) + 1.0f) * 0.5;
      _speed = MAXMOTORSPEED * (normalized * (_patternMaxValue - _patternMinValue) + _patternMinValue);
    }
    else
    {
      //We will do analogWrite either way. So nothing happens here.
    }
    analogWrite(_pin, _speed);
  }
  else
  {
    analogWrite(_pin, 0);
  }
}