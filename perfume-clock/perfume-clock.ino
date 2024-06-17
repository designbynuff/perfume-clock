#include <Wire.h>
#include "DS1307.h"

// Neopixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        4 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 10 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

DS1307 clock; // Define an object of DS1307 class

const int atomizerPins[] = {A0, A1, A2, A3, 2, 3, 4, 5, 6, 7}; // Pins for atomizers 0-9. Use in this order!

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

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  
  clock.begin();

  // Attempt to read the current time from the RTC
  clock.getTime(); // Retrieve time from RTC
  if (clock.hour >= 24 || clock.minute >= 60 || clock.second >= 60) {
    Serial.println("Couldn't get valid time from RTC, setting to fallback time 00:00:00");
    rtcAvailable = false;
    fallbackTime.hours = 0;
    fallbackTime.minutes = 0;
    fallbackTime.seconds = 0;
  } else {
    Serial.print("RTC found and time is set to: ");
    Serial.print(clock.hour);
    Serial.print(":");
    Serial.print(clock.minute);
    Serial.print(":");
    Serial.println(clock.second);
  }

  for (int i = 0; i < 10; i++) {
    pinMode(atomizerPins[i], OUTPUT);
    digitalWrite(atomizerPins[i], LOW);
  }
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  if (rtcAvailable) {
    clock.getTime();
    updateTime(clock.hour, clock.minute, clock.second);
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
    Serial.print("Turning on atomizer pixels.setPixelColor()currentNumber, pixels.COlor()202, 0, 150, 800;
  pixels.show();