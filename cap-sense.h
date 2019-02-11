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
  int readValue();
  void resetMinMaxCapValues();
  void logValues(bool newLine);
  void logValuesNormalized(bool newLine);

private:
  int _pin;
  int _minCapValue = 100000;
  // int _minCapValueCandidate = 0;
  int _maxCapValue = 0;
  // int _maxCapValueCandidate = 100000;
  int _filteredCapValue = 0;
  int _normalizedCapValue = 0;
  const float _Q_measure = 0.02;
};

#endif