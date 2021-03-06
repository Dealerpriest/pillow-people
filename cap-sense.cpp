#include "Arduino.h"
#include "cap-sense.h"
#include "touchAdvanced.c"

CapSense::CapSense(int pin) : _pin(pin) {}

void CapSense::initialize()
{
  _filteredCapValue = readRawValue();
}

void CapSense::resetMinMaxCapValues()
{
  _minCapValue = 100000;
  _maxCapValue = 0.0f;

  _filteredCapValue = readRawValue();
}

void CapSense::logValues(bool newLine = true)
{
  if (newLine)
  {
    Serial.printf("%i\t%i\t%i\n", _filteredCapValue, _minCapValue, int(_maxCapValue));
  }
  else
  {
    Serial.printf("%i\t%i\t%i\t", _filteredCapValue, _minCapValue, int(_maxCapValue));
  }
}

void CapSense::logValuesNormalized(bool newLine = true)
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

void CapSense::logDebugEvents(bool newLine = true)
{
  if (newLine)
  {
    Serial.printf("%i\t%i\t%i\n", slopeSamplingEvent, slopeNoThresholdEvent, noChangeDurationEvent);
  }
  else
  {
    Serial.printf("%i\t%i\t%i\t", slopeSamplingEvent, slopeNoThresholdEvent, noChangeDurationEvent);
  }

  slopeSamplingEvent = max(0, slopeSamplingEvent - 5);
  slopeNoThresholdEvent = max(0, slopeNoThresholdEvent - 5);
  noChangeDurationEvent = max(0, noChangeDurationEvent - 5);
}

void CapSense::autoCalibrate()
{  
  unsigned long now = millis();

  minValueExpanded = false;
  if (_filteredCapValue < _minCapValue)
  {
    _minCapValue = _filteredCapValue;

    minValueExpanded = true;
  }

  //Search and identify if we've been "still" long enough (i.e. the slope has been very small (under the threshold) for a long enough duration)
  if (now - _minValueSlopeStamp > _slopeSamplingInterval)
  {
    _minValueSlopeStamp = now;
    //Calculate slope since last slopeSampling.
    float slope = abs(_filteredCapValue - _previousFilteredCapvalue);
    
    //debug
    slopeSamplingEvent = 50;

    valueIsMoving = false;
    if (slope > _minCalSlopeThreshold)
    {
      //We have apparently moved too much to save as minvalue. Let's reset the "nomotion" timer.
      _minCalSearchStartStamp = now;

      valueIsMoving = true;
    }else{
      //debug
      slopeNoThresholdEvent = 200;

      
      minValueRecalculated = false;
      if (now - _minCalSearchStartStamp > _minCalNoSlopeDuration)
      {
        _minCapValue = _filteredCapValue;
        _minCalSearchStartStamp = now;
        
        //Debug
        noChangeDurationEvent = 400;
        minValueRecalculated = true;
      }
    }
    _previousFilteredCapvalue = _filteredCapValue;
  }

  maxValueExpanded = false;
  if (_filteredCapValue > int(_maxCapValue))
  {
    _maxCapValue = float(_filteredCapValue);
    _maxValueReachedStamp = now;

    _maxCalDecreaseSpeed = _maxCapValue - _minCapValue;
    _maxCalDecreaseSpeed /= 50000.f;

    maxValueExpanded = true;
  }

  maxValueShrinking = false;
  if (now - _maxValueReachedStamp > _maxCalNoReachDuration && _maxCapValue > _minCapValue + _minimalCalRange)
  {
    _maxCapValue -= _maxCalDecreaseSpeed;
    maxValueShrinking = true;
  }

  _maxCapValue = max(_minCapValue + _minimalCalRange, _maxCapValue);
}

int CapSense::readValue()
{
  int rawValue = readRawValue();
  _filteredCapValue = _Q_measure * (float)rawValue + (1.0f - _Q_measure) * _filteredCapValue;

  _normalizedCapValue = map(_filteredCapValue, _minCapValue, _maxCapValue, 0, 1000);
  return _normalizedCapValue;
}

int CapSense::readWithAutoCal()
{
  int rawValue = readRawValue();
  _filteredCapValue = _Q_measure * (float)rawValue + (1.0f - _Q_measure) * _filteredCapValue;
  autoCalibrate();
  _normalizedCapValue = map(_filteredCapValue, _minCapValue, _maxCapValue, 0, 1000);
  return readValue();
}

int CapSense::readRawValue()
{
  // current = 0 - 15
  // scans = 0 - 31
  // prescaler = 0 - 7
  //defaults are current = 2, nscans = 9, prescaler = 2
  return max(0, touchReadAdvanced(_pin, 15, 15, 2));
}