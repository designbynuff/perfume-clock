#include <Wire.h>
#include "DS1307.h"
#include <RTClib.h>

// Neopixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        8 // First available pin after atomizers

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 10 // Max LEDs in a column

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Rename this from 'clock' to 'rtc'
DS1307 rtc; // Define an object of DS1307 class

const int atomizerPins[] = {A0, A1, A2, A3, 2, 3, 4, 5, 6, 7}; // Pins for atomizers 0-9. Use in this order!

int currentDigit = 3; // H,h,M,m = 0,1,2,3
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

int currentNumber = -1;

unsigned long atomizerOnTime = 0;
const unsigned long atomizerDuration = 1000; // Duration the atomizer is on (in milliseconds)
bool atomizerActive = false;

// Forward declaration of incrementFallbackTime
void incrementFallbackTime();
void updateTime(int hours, int minutes, int seconds); // Forward declaration of updateTime

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  
  rtc.begin(); // Changed from clock.begin() to rtc.begin()

  // Attempt to read the current time from the RTC
  rtc.getTime(); // Retrieve time from RTC
  if (rtc.hour >= 24 || rtc.minute >= 60 || rtc.second >= 60) {
    Serial.println("Couldn't get valid time from RTC, setting to fallback time 00:00:00");
    rtcAvailable = false;
    fallbackTime.hours = 0;
    fallbackTime.minutes = 0;
    fallbackTime.seconds = 0;
  } else {
    Serial.print("RTC found and time is set to: ");
    Serial.print(rtc.hour);
    Serial.print(":");
    Serial.print(rtc.minute);
    Serial.print(":");
    Serial.println(rtc.second);
  }

  for (int i = 0; i < 10; i++) {
    pinMode(atomizerPins[i], OUTPUT);
    digitalWrite(atomizerPins[i], LOW);
  }
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  if (rtcAvailable) {
    rtc.getTime(); // Changed from clock.getTime() to rtc.getTime()
    updateTime(rtc.hour, rtc.minute, rtc.second); // Changed from clock.hour etc. to rtc.hour etc.
  } else {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      incrementFallbackTime();
    }
  }

  // Control Atomizer
  unsigned long currentMillis = millis();
  if (atomizerActive && (currentMillis - atomizerOnTime >= atomizerDuration)) {
    Serial.print("Turning off atomizer ");
    Serial.println(currentNumber);
    digitalWrite(atomizerPins[currentNumber], LOW); // Turn off the atomizer
    atomizerActive = false; // Reset the atomizer status
  }

  // Update LEDs
  if (currentNumber != -1) {
    pixels.setPixelColor(currentNumber, pixels.Color(200, 150, 80));
    pixels.show();
  }
}

void updateTime(int hours, int minutes, int seconds) {
  int H = hours / 10;
  int h = hours % 10;
  int M = minutes / 10;
  int m = minutes % 10;

  int digits[] = {H, h, M, m};
  currentNumber = digits[currentDigit]; // Get the digit for this column

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

  // Turn on the atomizer at the start of a new minute
  if (seconds == 0 && !atomizerActive) {
    Serial.print("Turning on atomizer ");
    Serial.println(currentNumber);
    digitalWrite(atomizerPins[currentNumber], HIGH); // Turn on the atomizer
    atomizerOnTime = millis(); // Record the time the atomizer was turned on
    atomizerActive = true; // Set the atomizer status to active
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
  Serial.print("Fallback time: ");
  Serial.print(fallbackTime.hours);
  Serial.print(":");
  Serial.print(fallbackTime.minutes);
  Serial.print(":");
  Serial.println(fallbackTime.seconds);
  updateTime(fallbackTime.hours, fallbackTime.minutes, fallbackTime.seconds);
}
