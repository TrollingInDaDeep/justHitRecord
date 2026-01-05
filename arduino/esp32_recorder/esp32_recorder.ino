#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "AudioTools.h"
// ESP32 Code to record audio from jack socket and save to SD Card

//ideas
//use touch button / touch pin https://www.programmingelectronics.com/esp32-capacitive-touch-button/

///
/// variables
///

//LED
int ledPin = 2;
int btnPin = 4;
bool isRecording = false; //if recording is armed
bool stateChanged = false; //if the button state already changed
int debounce = 10; //button debounce time in ms

//Audio I2s PCM1808 stuff
AudioInfo info(44100, 2, 32); //samplerate
I2SStream i2sStream;
CsvOutput<int32_t> csvOutput(Serial);
StreamCopy copier(csvOutput, i2sStream); //Copy audiostream to CSVoutput
int i2s_bck = 15; //D15 (Pin 15) BCK (Bit Clock)
int i2s_ws = 16; //RX2 (Pin 16) WS (LRC = left right clock)
int i2s_data = 17; //TX2 (Pin 17) Data (out)
int i2s_mck = 3; //RX0 (Pin 3) MCLK (SCK)


void setup() {

  //LED Pin Modes
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
  Serial.begin(9600);

  auto cfg = i2sStream.defaultConfig(RX_MODE);
    cfg.copyFrom(info);
    cfg.pin_bck = i2s_bck;
    cfg.pin_ws = i2s_ws;
    cfg.pin_data = i2s_data;
    cfg.pin_mck = i2s_mck; // optional master clock pin


    cfg.i2s_format = I2S_STD_FORMAT; // default format
    cfg.is_master = true; // default esp32 is master
    cfg.use_apll = false; //try with true

  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);
  i2sStream.begin(cfg);
  csvOutput.begin(info);

}

void loop() {
  copier.copy(); //copy audio to CSV stream
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