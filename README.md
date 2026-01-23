# justHitRecord
I want to record my instruments in an easy and compact format. Just plug in the aux cable, click a button, it records onto an SD card.
While recording an LED lights up and goes dark when the recording finishes.

Current plan:
Jack socket -> PCM ADC -> ESP32 -> SD Breakout board -> SD Card
Additionally, wire a button and an LED to the ESP for controlling.

Future improvements
Hardware
* better fitting case
* SD card doesn't necessarily need to be exposed
* button retreated into case, not sticking out
* shorter or less stiffer cables
* generally smaller
* PCB to stick everything onto
* smaller components, not breakout boards
* sort of RTC so that the files have a correct Date

Software
* Recording format: change from RAW to WAV or mp3. change samplerate to 320kb/s to make handling easier
* "Vu Meter" while recording, LED dims to display the current recording level. So you can see if a signal is coming in (and you did not forget to turn up the volume on the recording output, which totally didn't happen)
* LED blink indications (errors -> LED flashing, Recording start -> 2 flashes, etc...)
* longpress to end recording
* Make esp connect to wifi and be able to get the recording files via network
* Mak esp be recognized as usb drive when plugged in to get files without removing SD card
* Cleanup empty folders or files that are shorter than 1-2 seconds to make extraction process easier.
* sort of RTC so that the files have a correct Date
