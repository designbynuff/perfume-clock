#include <Wire.h>
#include "DS1307.h"

// Define pins for each atomizer from digit 0-9 (not sure how this works on a Grove Shield)
// #define mist_zero
// #define mist_one
// #define mist_two
// #define mist_three
// #define mist_four
// #define mist_five
// #define mist_six
// #define mist_seven
// #define mist_eight
// #define mist_nine

// Let's try it as an array for now, will figure out pin mapping soon
const int atomizerPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // Pins for atomizers 0-9

// Global Variables
RTC_DS3231 rtc;
int currentDigit = 4; // Will be updated based on column
int prevSeconds = -1;
int prevH = -1, prevh = -1, prevM = -1, prevm = -1;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC, setting time to 00:00:00");
    rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
  }
  else if (rtc.lostPower())
  {
    Serial.println("RTC lost power, setting time to current compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  for (int i = 0; i < 10; i++)
  {
    pinMode(atomizerPins[i], OUTPUT);
    digitalWrite(atomizerPins[i], LOW);
  }
}

void loop()
{
  DateTime now = rtc.now();

  int hours = now.hour();
  int minutes = now.minute();
  int seconds = now.second();

  int H = hours / 10;
  int h = hours % 10;
  int M = minutes / 10;
  int m = minutes % 10;

  int digits[] = {H, h, M, m};
  int currentNumber = digits[currentDigit]; // Get the digit for this column

  // Print the current time if it changes
  if (seconds != prevSeconds)
  {
    Serial.print("Current time: ");
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
    prevSeconds = seconds;
  }

  // Print the current digits if they change
  if (H != prevH || h != prevh || M != prevM || m != prevm)
  {
    Serial.print("HhMm: ");
    Serial.print(H);
    Serial.print(h);
    Serial.print(M);
    Serial.println(m);
    prevH = H;
    prevh = h;
    prevM = M;
    prevm = m;
  }

  // Control Atomizer
  if (seconds == 0)
  {
    Serial.print("Turning on atomizer ");
    Serial.println(currentNumber);
    digitalWrite(atomizerPins[currentNumber], HIGH); // Turn on the atomizer
    delay(1000);                                     // Keep it on for one second
    digitalWrite(atomizerPins[currentNumber], LOW);  // Turn off the atomizer
  }
}
