#include "FS.h"
#include "SD.h"
#include "SPI.h"

// ESP32 Code to record audio from jack socket and save to SD Card

//ideas
//use touch button / touch pin https://www.programmingelectronics.com/esp32-capacitive-touch-button/

///
/// variables
///
int ledPin = 2;
int btnPin = 4;
bool isRecording = false; //if recording is armed
bool stateChanged = false; //if the button state already changed
int debounce = 10; //button debounce time in ms

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(btnPin)) {
    if (!stateChanged) {
      isRecording = !isRecording;
      digitalWrite(ledPin, isRecording);
      Serial.println(isRecording);
      stateChanged = true;
    }
    
  }
  if (!digitalRead(btnPin)) {
    stateChanged = false;
  }
  delay(debounce);

}