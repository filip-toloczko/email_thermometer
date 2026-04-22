#include <Arduino.h>
#include <WiFi.h>
#include <secrets.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

