#define DEBUG
#ifndef DEBUG
#define PRINT_LN(x)
#define PRINT(x)
#else
#define PRINT_LN(x) Serial.println(x)
#define PRINT(x) Serial.print(x)
#endif

int touchPin = 15;
int pwmLedPin = 23;
int pwmValue = 0;
unsigned long heartStamp;
unsigned long heartInterval = 500;
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

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

unsigned long autoCalMinStamp;
unsigned long autoCalMaxStamp;
unsigned long autoCalInterval = 10000; // Make sure this is more than the longest in milliseconds you expect to touch
int minBite = 100000;
int minBiteCandidate = 0;
int maxBite = 0;
int maxBiteCandidate = 100000;
int safeDistanceMin = 100;
int safeDistanceMax = 50;
bool biteFirstZero = true; // a boolean to ensure always sending one zero value before neglecting data.
float Q_measure = 0.2;
int filteredCapValue = 0;

int capValue;
void loop()
{
  // Serial.printf("%i,", touchRead(touchPin));

  capValue = readCapPin(touchPin);
  filteredCapValue = Q_measure * (float)capValue + (1.0f - Q_measure) * filteredCapValue;
  pwmValue = map(filteredCapValue, 0, 1000, 0, 255);

  analogWrite(pwmLedPin, pwmValue);
  // Serial.println();
  Serial.printf("%i\t%i\t%i\t%i\t%i\n", capValue, filteredCapValue, minBite, maxBite, pwmValue);

  delay(10);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  if (millis() - heartStamp > heartInterval)
  {
    heartStamp = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // delay(5);
  }
}

int readCapPin(int pin)
{
  //int readingPos = max(0, biteSensor.capacitiveSensor(30));
  // PRINT(readingPos);
  // PRINT('\t');
  //int readingNeg = max(0, biteSensorNeg.capacitiveSensor(30));
  // PRINT(readingNeg);
  // PRINT('\t');

  //Calculate new minBite
  int reading = max(0, touchRead(pin));
  if (reading + safeDistanceMin < minBite)
  {
    minBite = reading + safeDistanceMin;
  }

  //If the minbite is fucking old. Choose the candidate as new minbite
  if (millis() > autoCalMinStamp + autoCalInterval)
  {
    // Serial.println("recalibrating min");
    autoCalMinStamp = millis();
    if (minBiteCandidate > minBite)
    { //Is the minBite super low? Move it up to minbiteCandidate.
      minBite = minBiteCandidate;
    }
    else
    {
      minBiteCandidate = minBite; //else. choose new candidate for future comparisons
    }
  }

  if (reading - safeDistanceMax > maxBite)
  {
    maxBite = reading - safeDistanceMax;
  }
  if (millis() > autoCalMaxStamp + autoCalInterval)
  {
    // Serial.println("recalibrating max");
    autoCalMaxStamp = millis();
    if (maxBiteCandidate < maxBite)
    {
      maxBite = maxBiteCandidate;
    }
    else
    {
      maxBiteCandidate = maxBite;
    }
  }
  //PRINT('\t');
  //PRINT(readingPos);
  //int readingNeg = max(0, touchRead(22));
  //PRINT('\t');
  //PRINT(readingNeg);
  //int reading = readingPos - readingNeg;

  // PRINT('\t');
  // PRINT(reading);
  // PRINT('\t');
  // PRINT(minBite);
  // PRINT('\t');
  // PRINT(maxBite);
  // PRINT_LN();
  if (reading < minBite && biteFirstZero)
  {
    biteFirstZero = false;
    return 0;
  }
  else if (reading < minBite)
  {
    return -1;
  }
  else
  {
    biteFirstZero = true;
    return map(reading, minBite, maxBite, 0, 1000);
  }
}