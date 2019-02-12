#ifndef CAP_SENSE_H
#define CAP_SENSE_H

//TODO:
// fixa så man kan starta/stoppa motorn
// fixa så man kan stänga av/på "intervalvågen"
//

class CapSense
{
public:
  CapSense(int pin);
  void initialize();
  void autoCalibrate();
  int readValue();
  int readWithAutoCal();
  void resetMinMaxCapValues();
  void logValues(bool newLine);
  void logValuesNormalized(bool newLine);

  //Debug variables
  void logDebugEvents(bool newLine);
  int slopeSamplingEvent = 0;
  int slopeThresholdEvent = 0;
  int noChangeDurationEvent = 0;

private:
  int readRawValue();

  int _pin;
  int _minCapValue = 100000;
  // int _minCapValueCandidate = 0;
  int _maxCapValue = 0;

  int _minimalCalRange = 50;
  // int _maxCapValueCandidate = 100000;
  int _filteredCapValue = 0;
  int _normalizedCapValue = 0;
  const float _Q_measure = 0.02;

  float _previousFilteredCapvalue = 0;

  unsigned long _minCalSearchStartStamp = 0;
  unsigned long _minValueSlopeStamp = 0;
  unsigned long _slopeSamplingInterval = 250;
  float _minCalSlopeThreshold = 5.f;
  unsigned long _minCalNoSlopeDuration = 20000;

  unsigned long _maxValueReachedStamp = 0;
  unsigned long _maxCalNoReachDuration = 10000;
  int _maxCalDecreaseSpeed = 1;
};

#endif