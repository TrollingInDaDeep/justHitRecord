/*
  What board to select in arduino IDE
  // esponly: DOIT ESP32 DEVKIT V1
  // audiokit: ESP32 DEV Module
  this code shall test reading audio signal
  [audio source] -> jack connector -> PCM1808 -> ESP -> display value in serial monitor

  https://github.com/marcopyre/esp32_pcm1808_SoundRecorder?tab=readme-ov-file
  https://www.youtube.com/watch?v=pvU198ZbWpI
  
  
  ESP32 Audio Kit V2.2 2957
  has an
  5 ai_thinker (ES8388) 2957 3478
  according to https://github.com/pschatzmann/arduino-audiokit/wiki/GPIO-Overview-by-Selected-Board

  ES8388 has Up to 96 kHz 24 bpp high quality audio according to 
  https://pcbartists.com/product/es8388-module/?srsltid=AfmBOopYfTEhCkb7qouU3XUckY5O35GDnG7K8EfXL_3udVEY7ieINUTb

  big ups for pschatzmann
  Buy me a coffee https://www.buymeacoffee.com/philschatzh

  example: https://github.com/pschatzmann/arduino-audio-driver/wiki/Stand%E2%80%90Alone:-Predefined-Boards
https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-audiokit/streams-audiokit-audiokit/streams-audiokit-audiokit.ino
  AudioKitAC101 <-- USE THIS
  AudioKitEs8388V1
  AudioKitEs8388V2
*/
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"

AudioBoardStream kit(AudioKitAC101); // Access I2S as stream
StreamCopy copier(kit, kit); // copy kit to kit

// Arduino Setup
void setup(void) {
    Serial.begin(115200);
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
    
    auto cfg = kit.defaultConfig(RXTX_MODE);
    cfg.sd_active = false;
    cfg.input_device = ADC_INPUT_LINE2;
    kit.begin(cfg);
    kit.setVolume(10); //here not working
}

// Arduino loop - copy data
void loop() {
    copier.copy();
}