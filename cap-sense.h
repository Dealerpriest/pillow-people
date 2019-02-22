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
  int slopeNoThresholdEvent = 0;
  int noChangeDurationEvent = 0;

  bool valueIsMoving = false;
  bool minValueExpanded = false;
  bool minValueRecalculated = false;
  bool maxValueShrinking = false;
  bool maxValueExpanded = false;

private:
  int readRawValue();

  int _pin;
  int _minCapValue = 100000;
  // int _minCapValueCandidate = 0;
  float _maxCapValue = 0;

  int _minimalCalRange = 50;
  // int _maxCapValueCandidate = 100000;
  int _filteredCapValue = 0;
  int _normalizedCapValue = 0;
  const float _Q_measure = 0.02;

  float _previousFilteredCapvalue = 0;

  // calibration stuff
  unsigned long _minCalSearchStartStamp = 0;
  unsigned long _minValueSlopeStamp = 0;
  unsigned long _slopeSamplingInterval = 250;
  float _minCalSlopeThreshold = 3.f;
  unsigned long _minCalNoSlopeDuration = 20000;

  unsigned long _maxValueReachedStamp = 0;
  unsigned long _maxCalNoReachDuration = 300000;
  float _maxCalDecreaseSpeed = 1;
};

#endif