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


void setup() {
  Wire.begin();
  rtc.begin();
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  for (int i = 0; i < 10; i++) {
    pinMode(atomizerPins[i], OUTPUT);
    digitalWrite(atomizerPins[i], LOW);
  }
}

void loop() {
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
  
  // Control Atomizer
  if (seconds == 0) {
    digitalWrite(atomizerPins[currentNumber], HIGH); // Turn on the atomizer
    delay(1000); // Keep it on for one second
    digitalWrite(atomizerPins[currentNumber], LOW); // Turn off the atomizer
  }
  
  delay(1000); // Update every second
}

