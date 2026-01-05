/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-microsd-card-arduino/
  
  This sketch can be found at: Examples > SD(esp32) > SD_Test

  pinout esp: https://www.reddit.com/media?url=https%3A%2F%2Fpreview.redd.it%2F0u14t38nxeo21.png%3Fwidth%3D1200%26format%3Dpng%26auto%3Dwebp%26s%3Dcaa046ee9ff289bb55fa8b3a47eae33c4db876bd
  pin connections:
  3v -> 3v
  GND -> GND
  CLK -> D18
  (MI)SO -> D19
  (MO)SI -> D23
  CS -> D5

*/

#include "FS.h"
#include "SD.h"
#include "SPI.h"

void setup(){
  sd_setup();
}

void loop(){
  sd_loop();
}