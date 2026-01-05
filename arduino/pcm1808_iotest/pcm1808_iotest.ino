/*
  pcm1808
  5v -> vin
  3.3 -> 3.3
  GND -> GND
  md0, md1, fmy -> GND
  bck -> D15 (Pin 15) BCK
  out -> TX2 (Pin 17) data
  LRC -> RX2 (Pin 16) WS
  SCK -> RX0 (Pin 3) MCLK

  Example pschatzmann
  https://github.com/pschatzmann/arduino-audio-tools/discussions/2172
  https://github.com/pschatzmann/arduino-audio-tools/wiki/External-ADC

  https://www.youtube.com/watch?v=pvU198ZbWpI&
*/
#include "AudioTools.h"

AudioInfo info(44100, 2, 32);
I2SStream i2sStream;
CsvOutput<int32_t> csvOutput(Serial);
StreamCopy copier(csvOutput, i2sStream);


void setup() {
  auto cfg = i2sStream.defaultConfig(RX_MODE);
    cfg.copyFrom(info);
    cfg.pin_bck = 15;
    cfg.pin_ws = 16;
    cfg.pin_data = 17;
    cfg.pin_mck = 3; // optional master clock pin


    cfg.i2s_format = I2S_STD_FORMAT; // default format
    cfg.is_master = true; // default esp32 is master
    cfg.use_apll = false; //try with true

  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  i2sStream.begin(cfg);

  csvOutput.begin(info);

}

void loop() {
  copier.copy();
}
