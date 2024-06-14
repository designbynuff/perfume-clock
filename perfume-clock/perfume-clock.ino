#include <Wire.h>
#include "DS1307.h"
#include <RTClib.h>

const int atomizerPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // Pins for atomizers 0-9

RTC_DS3231 rtc;
int currentDigit = 0; // Will be updated based on column
int prevSeconds = -1;
int prevH = -1, prevh = -1, prevM = -1, prevm = -1;

bool rtcAvailable = true;
unsigned long previousMillis = 0;
unsigned long interval = 1000;

struct FallbackTime {
  int hours;
  int minutes;
  int seconds;
} fallbackTime;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC, using fallback timekeeping");
    rtcAvailable = false;
    fallbackTime.hours = 0;
    fallbackTime.minutes = 0;
    fallbackTime.seconds = 0;
  } else if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time to current compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  for (int i = 0; i < 10; i++) {
    pinMode(atomizerPins[i], OUTPUT);
    digitalWrite(atomizerPins[i], LOW);
  }
}

void loop() {
  if (rtcAvailable) {
    DateTime now = rtc.now();
    updateTime(now.hour(), now.minute(), now.second());
  } else {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      incrementFallbackTime();
    }
  }
}

void updateTime(int hours, int minutes, int seconds) {
  int H = hours / 10;
  int h = hours % 10;
  int M = minutes / 10;
  int m = minutes % 10;

  int digits[] = {H, h, M, m};
  int currentNumber = digits[currentDigit]; // Get the digit for this column

  // Print the current time if it changes
  if (seconds != prevSeconds) {
    Serial.print("Current time: ");
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
    prevSeconds = seconds;
  }

  // Print the current digits if they change
  if (H != prevH || h != prevh || M != prevM || m != prevm) {
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
  if (seconds == 0) {
    Serial.print("Turning on atomizer ");
    Serial.println(currentNumber);
    digitalWrite(atomizerPins[currentNumber], HIGH); // Turn on the atomizer
    delay(1000); // Keep it on for one second
    digitalWrite(atomizerPins[currentNumber], LOW); // Turn off the atomizer
  }
}

void incrementFallbackTime() {
  fallbackTime.seconds++;
  if (fallbackTime.seconds >= 60) {
    fallbackTime.seconds = 0;
    fallbackTime.minutes++;
    if (fallbackTime.minutes >= 60) {
      fallbackTime.minutes = 0;
      fallbackTime.hours++;
      if (fallbackTime.hours >= 24) {
        fallbackTime.hours = 0;
      }
    }
  }
  updateTime(fallbackTime.hours, fallbackTime.minutes, fallbackTime.seconds);
}
