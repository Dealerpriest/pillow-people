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

const int resetCalibrationPin = 2;
int touchPin = 15;
int pwmLedPin = 23;
int pwmValue = 0;
unsigned long heartStamp;
unsigned long heartInterval = 500;

CapSense capPins[2] = {CapSense(15), CapSense(16)};

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(resetCalibrationPin, INPUT_PULLUP);

  Serial.begin(57600);
  // while (!Serial.dtr()){
  //   if(millis() > serialWaitTimeout){
  //     break;
  //   }
  // }
  digitalWrite(LED_BUILTIN, LOW);
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(50);
  }
  heartStamp = millis();
}

// int capValue;
void loop()
{
  // Serial.printf("%i,", touchRead(touchPin));

  // readCapPin(touchPin);
  // filteredCapValue = Q_measure * (float)capValue + (1.0f - Q_measure) * filteredCapValue;
  // pwmValue = map(filteredCapValue, 0, 1000, 0, 255);

  capPins[0].readValue();
  capPins[0].logValuesNormalized(false);

  capPins[1].readValue();
  capPins[1].logValuesNormalized(false);

  Serial.printf("1000\n");

  analogWrite(pwmLedPin, pwmValue);
  // Serial.println();
  // Serial.printf("%i\t%i\t%i\t%i\t%i\n", capValue, filteredCapValue, minCapValue, maxCapValue, pwmValue);

  delay(10);

  if (millis() - heartStamp > heartInterval)
  {
    heartStamp = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // delay(5);
  }

  if (millis() < 2000 || !digitalRead(resetCalibrationPin))
  {
    capPins[0].resetMinMaxCapValues();
    capPins[1].resetMinMaxCapValues();
    resetCapCalibration();
  }
}

unsigned long autoCalMinStamp;
unsigned long autoCalMaxStamp;
unsigned long autoCalInterval = 10000; // Make sure this is more than the longest in milliseconds you expect to touch
int minCapValue = 100000;
int minCapValueCandidate = 0;
int maxCapValue = 0;
int maxCapValueCandidate = 100000;
int safeDistanceMin = 0;
int safeDistanceMax = 0;
int filteredCapValue = 0;

// bool biteFirstZero = true; // a boolean to ensure always sending one zero value before neglecting data.
void resetCapCalibration()
{
  minCapValue = 100000;
  maxCapValue = 0;
}

// int readCapPin(int pin)
// {
//   //Calculate new minCapValue
//   //defaults are current = 2, nscans = 9, prescaler = 2
//   int rawValue = max(0, touchReadAdvanced(pin, 15, 30, 2));
//   float Q_measure = 0.02;
//   filteredCapValue = Q_measure * (float)rawValue + (1.0f - Q_measure) * filteredCapValue;

//   if (filteredCapValue < minCapValue)
//   {
//     minCapValue = filteredCapValue;
//   }

//   //If the minCapValue is fucking old. Choose the candidate as new minCapValue
//   // if (millis() > autoCalMinStamp + autoCalInterval)
//   // {
//   //   // Serial.println("recalibrating min");
//   //   autoCalMinStamp = millis();
//   //   if (minCapValueCandidate > minCapValue)
//   //   { //Is the minCapValue super low? Move it up to minCapValueCandidate.
//   //     minCapValue = minCapValueCandidate;
//   //   }
//   //   else
//   //   {
//   //     minCapValueCandidate = minCapValue; //else. choose new candidate for future comparisons
//   //   }
//   // }

//   if (filteredCapValue > maxCapValue)
//   {
//     maxCapValue = filteredCapValue;
//   }

//   // if (millis() > autoCalMaxStamp + autoCalInterval)
//   // {
//   //   // Serial.println("recalibrating max");
//   //   autoCalMaxStamp = millis();
//   //   if (maxCapValueCandidate < maxCapValue)
//   //   {
//   //     maxCapValue = maxCapValueCandidate;
//   //   }
//   //   else
//   //   {
//   //     maxCapValueCandidate = maxCapValue;
//   //   }
//   // }

//   //PRINT('\t');
//   //PRINT(readingPos);
//   //int readingNeg = max(0, touchRead(22));
//   //PRINT('\t');
//   //PRINT(readingNeg);
//   //int reading = readingPos - readingNeg;

//   // PRINT('\t');
//   // PRINT(reading);
//   // PRINT('\t');
//   // PRINT(minCapValue);
//   // PRINT('\t');
//   // PRINT(maxCapValue);
//   // PRINT_LN();

//   Serial.printf("%i\t%i\t%i\n", filteredCapValue, minCapValue, maxCapValue);

//   // if (reading < minCapValue && biteFirstZero)
//   // {
//   //   // biteFirstZero = false;
//   //   return 0;
//   // }
//   // else if (reading < minCapValue)
//   // {
//   //   return -1;
//   // }
//   // else
//   // {
//   // biteFirstZero = true;
//   return map(filteredCapValue, minCapValue, maxCapValue, 0, 1000);
//   // }
// }