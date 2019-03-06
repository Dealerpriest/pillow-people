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

#include "Bounce.h"

// const int resetCalibrationPin = 2;
// const int motorActivationPin = 4;
//int touchPin = 15;
//int pwmLedPin = 23;
//int pwmValue = 0;
unsigned long heartStamp;
unsigned long heartDefaultInterval = 500;
unsigned long heartCurrentInterval = 500;

const int ledPin1 = 1;
const int ledPin2 = 2;

CapSense capPins[] = {
  CapSense(15), CapSense(16), CapSense(17), CapSense(18), 
  CapSense(19), CapSense(22), CapSense(23), CapSense(0)
  };
const int nrOfCapPins = sizeof(capPins) / sizeof(CapSense);
// const int nrOfCapPins = 1;

MotorControl motorPins[] = {
  MotorControl(10), MotorControl(9), MotorControl(20), MotorControl(21), 
  MotorControl(6), MotorControl(5), MotorControl(4), MotorControl(3)
  };
const int nrOfMotorPins = sizeof(motorPins) / sizeof(MotorControl);

Bounce buttons[3] = { Bounce(11, 8), Bounce(12, 8), Bounce(14, 8) };
const int nrOfButtons = sizeof(buttons) / sizeof(Bounce);

bool runMotors = true;

enum Mode {normal, ledDebugging};
Mode currentMode = normal;


unsigned long minValueRecalculatedStamp = 0;
unsigned long led1BlinkStamp = 0;
unsigned long led1BlinkInterval = 15;
unsigned long led2BlinkStamp = 0;
unsigned long led2BlinkInterval = 15;

//Threshold ett värde mellan 0 och 1000. Definerar övergång mellan patterns
int lightPatternThreshold = 200; // 100
int mediumPatternThreshold = 500; // 400
int heavyPatternThreshold = 750; // 850

//Hur lång tid en svängning tar i sekunder.
float lightPatternInterval = 5.0f;
float mediumPatternInterval = 2.5f;
float heavyPatternInterval = 5.0f;

//Amplituden för pattern för min och max. Ett värde mellan 0 - 1

float lightPatternMinIntensity = 0.05f;
float lightPatternMaxIntensity = 0.2f;

float mediumPatternMinIntensity = 0.2f;
float mediumPatternMaxIntensity = 0.6f;

float heavyPatternMinIntensity = 0.0f;
float heavyPatternMaxIntensity = 1.0f;

void setup()
{
  pinMode(11,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(14,INPUT_PULLUP);
  
  pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, HIGH);

  pinMode(ledPin1, OUTPUT);
  // digitalWrite(ledPin1, HIGH);
  
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, HIGH);

  for (int i = 0; i < nrOfCapPins; ++i)
  {
    capPins[i].initialize();
  }

   for (int j = 0; j < 50; j++) {
      for (int i = 0; i < nrOfButtons; i++) {
        buttons[i].update();
        buttons[i].risingEdge();
        buttons[i].fallingEdge();
      }
   }

  Serial.begin(57600);

  digitalWrite(LED_BUILTIN, LOW);
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(50);
  }
  heartStamp = millis();

  minValueRecalculatedStamp = millis();

  for (int i = 0; i < nrOfMotorPins; i++)
  {
    motorPins[i].initialize();
    motorPins[i].turnOn();
    motorPins[i].activatePattern();
    motorPins[i].setPatternMinValue(0.f);
    motorPins[i].setPatternMaxValue(1.f);
    motorPins[i].setFrequency(1.5 * (float(i)*0.05+1.0f));
    //motorPins[i].deactivatePattern();
  }

  digitalWrite(ledPin2, LOW);
}

void loop()
{
  for(int i = 0; i < nrOfButtons; i++)
  {
    buttons[i].update();
  }

  //toggle motors
  if(buttons[0].risingEdge()){
    runMotors = !runMotors;
    // Serial.printf("rising edge\n");
    // Serial.printf("runMotors: %i", int(runMotors));
    if(runMotors){
      turnOnAllMotors();
    } else {
      turnOffAllMotors();
    }
  }

  if(buttons[1].risingEdge()){
     blockingTestMotors();
  }

  if(buttons[2].risingEdge()){
    blockingBlink(50, 20);
    resetAllCapCalibration();
  }

  
  for (int i = 0; i < nrOfMotorPins; i++)
  {
    motorPins[i].update();
  }

  heartCurrentInterval = heartDefaultInterval;
  led1BlinkInterval = 0;
  led2BlinkInterval = 0;
  bool maxValueShrinking = false;
  for (int i = 0; i < nrOfCapPins; i++)
  {
    int capValue = capPins[i].readWithAutoCal();
    // capPins[i].logDebugEvents(false);
    capPins[i].logValuesNormalized(false);
    //capPins[i].logValues(false);

    
    // LED DEBUGGING
    if(capPins[i].valueIsMoving){
      heartCurrentInterval = 15;
    }
    if(capPins[i].minValueRecalculated){
      minValueRecalculatedStamp = millis();
    }
    if(capPins[i].minValueExpanded){
      led1BlinkInterval = 40;
    }
    if(capPins[i].maxValueShrinking){
      maxValueShrinking = true;
    }
    if(capPins[i].maxValueExpanded){
      led2BlinkInterval = 40;
    }


    // SET PATTERNS
    float intensity = float(capValue);
    if (intensity < lightPatternThreshold){
      motorPins[i].setMotorSpeed(0);
    } else {
      //motorPins[i].();
      motorPins[i].activatePattern();
    }

    //motorPins[i].activatePattern();
    
    if(intensity < mediumPatternThreshold){
      motorPins[i].setInterval(lightPatternInterval);
      motorPins[i].setPatternMinValue(lightPatternMinIntensity);
      motorPins[i].setPatternMaxValue(lightPatternMaxIntensity);
    } else if(intensity < heavyPatternThreshold){
      motorPins[i].setInterval(mediumPatternInterval);
      motorPins[i].setPatternMinValue(mediumPatternMinIntensity);
      motorPins[i].setPatternMaxValue(mediumPatternMaxIntensity);
    } else { 
      // motorPins[i].deactivatePattern();
      // motorPins[i].setMotorSpeed(0.15f);

      motorPins[i].setInterval(heavyPatternInterval);
      motorPins[i].setPatternMinValue(heavyPatternMinIntensity);
      motorPins[i].setPatternMaxValue(heavyPatternMaxIntensity);
    }
    

    // motorPins[i].setPatternMaxValue(intensity);
  }

  if(millis() - minValueRecalculatedStamp < 1000){
    digitalWrite(ledPin1, HIGH);  
  }else{
    digitalWrite(ledPin1, LOW);
  }

  if(led1BlinkInterval != 0 && millis() - led1BlinkStamp > led1BlinkInterval){
    led1BlinkStamp = millis();
    digitalWrite(ledPin1, !digitalRead(ledPin1));
  }

  if(led2BlinkInterval != 0){
    if(millis() - led2BlinkStamp > led2BlinkInterval){
      led2BlinkStamp = millis();
      digitalWrite(ledPin2, !digitalRead(ledPin2));
    }
  }
  else{
    if(maxValueShrinking){
      digitalWrite(ledPin2, HIGH);
    }else{
      digitalWrite(ledPin2, LOW);
    }
  }
  

  Serial.println();
  // Serial.printf("1000\n");

  delay(3);

  if (millis() < 2000)
  {
    resetAllCapCalibration();
  }

  // HEART BEAT ******************************
  if (millis() - heartStamp > heartCurrentInterval)
  {
    heartStamp = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}


// TODO: Make this shit proper!!
int getMappedIntensityValue(int input, int inputMargin, int outputCenter) {
  return map(input, input-inputMargin, input+inputMargin, 0, outputCenter);
}

void turnOnAllMotors(){
  for(int i = 0; i < nrOfMotorPins; i++)
  {
    motorPins[i].turnOn();
  }
  
}

void turnOffAllMotors(){
  for(int i = 0; i < nrOfMotorPins; i++)
  {
    motorPins[i].turnOff();
  }
  
}

void resetAllCapCalibration()
{
  for (int i = 0; i < nrOfCapPins; i++)
  {
    capPins[i].resetMinMaxCapValues();
  }
}

// be aware! this functions halts all other execution when called.
void blockingBlink(int blinks, int millis) {
  for (int i = 0; i < blinks; i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(millis);
  }
}

void blockingTestMotors() {
    turnOffAllMotors();
    for(int i = 0; i < nrOfMotorPins; i++)
      {
        motorPins[i].update();
      }

    for (int i = 0; i < nrOfMotorPins; i++)
      {
        motorPins[i].turnOn();
        motorPins[i].setMotorSpeed(0.5);
        motorPins[i].update();
        delay(1500);

        motorPins[i].setMotorSpeed(0);
        //motorPins[i].turnOff();
         motorPins[i].update();
        //motorPins[i].activatePattern();
      }
}