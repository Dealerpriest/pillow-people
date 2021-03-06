#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

//TODO:
// fixa så man kan starta/stoppa motorn
// fixa så man kan stänga av/på "intervalvågen"
//

class MotorControl
{
public:
  MotorControl(int pin);
  void initialize();
  void setInterval(float interval);
  void setFrequency(float frequency);
  void setPatternMinValue(float minValue);
  void setPatternMaxValue(float maxValue);
  void activatePattern();
  void deactivatePattern();

  void setMotorSpeed(float speed);
  void turnOn();
  void turnOff();

  void update();

private:
  int _pin;
  int _speed; // current power of motor
  float _frequency;
  float _phase = 0.0f;
  float _updateStamp = 0;
  float _prevUpdateStamp = 0;
  bool _runPattern = false;
  float _patternMaxValue = 1.0f;
  float _patternMinValue = 0.0f;

  bool _runMotor = false;

  const int MAXMOTORSPEED = 255;
};

#endif