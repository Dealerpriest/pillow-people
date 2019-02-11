#include "Arduino.h"
#include "cap-sense.h"
#include "touchAdvanced.c"

CapSense::CapSense(int pin) : _pin(pin) {}

void CapSense::resetMinMaxCapValues()
{
  _minCapValue = 100000;
  _maxCapValue = 0;
}

void CapSense::logValues(bool newLine)
{
  if (newLine)
  {
    Serial.printf("%i\t%i\t%i\n", _filteredCapValue, _minCapValue, _maxCapValue);
  }
  else
  {
    Serial.printf("%i\t%i\t%i\t", _filteredCapValue, _minCapValue, _maxCapValue);
  }
}

void CapSense::logValuesNormalized(bool newLine)
{
  if (newLine)
  {
    Serial.printf("%i\n", _normalizedCapValue);
  }
  else
  {
    Serial.printf("%i\t", _normalizedCapValue);
  }
}

int CapSense::readValue()
{
  //Calculate new minCapValue
  //defaults are current = 2, nscans = 9, prescaler = 2
  int rawValue = max(0, touchReadAdvanced(_pin, 15, 30, 2));
  _filteredCapValue = _Q_measure * (float)rawValue + (1.0f - _Q_measure) * _filteredCapValue;

  if (_filteredCapValue < _minCapValue)
  {
    _minCapValue = _filteredCapValue;
  }

  //If the minCapValue is fucking old. Choose the candidate as new minCapValue
  // if (millis() > autoCalMinStamp + autoCalInterval)
  // {
  //   // Serial.println("recalibrating min");
  //   autoCalMinStamp = millis();
  //   if (minCapValueCandidate > minCapValue)
  //   { //Is the minCapValue super low? Move it up to minCapValueCandidate.
  //     minCapValue = minCapValueCandidate;
  //   }
  //   else
  //   {
  //     minCapValueCandidate = minCapValue; //else. choose new candidate for future comparisons
  //   }
  // }

  if (_filteredCapValue > _maxCapValue)
  {
    _maxCapValue = _filteredCapValue;
  }

  // if (millis() > autoCalMaxStamp + autoCalInterval)
  // {
  //   // Serial.println("recalibrating max");
  //   autoCalMaxStamp = millis();
  //   if (maxCapValueCandidate < maxCapValue)
  //   {
  //     maxCapValue = maxCapValueCandidate;
  //   }
  //   else
  //   {
  //     maxCapValueCandidate = maxCapValue;
  //   }
  // }
  _normalizedCapValue = map(_filteredCapValue, _minCapValue, _maxCapValue, 0, 1000);
  return _normalizedCapValue;
}