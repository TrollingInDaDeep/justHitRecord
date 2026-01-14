#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include <WiFi.h>
#include <credentials.h>
// ESP32 Code to record audio from jack socket and save to SD Card

//ideas
//use touch button / touch pin https://www.programmingelectronics.com/esp32-capacitive-touch-button/

//used examples:
//https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-audiokit/streams-audiokit-sd-audiokit/streams-audiokit-sd-audiokit.ino

///
/// Function initializers
///
bool sd_setup();
void sd_loop();
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);
void createSessionDir(fs::FS &fs, const char * dirname, uint8_t levels);
void setNextRecordingName(fs::FS &fs, const char* dirname, uint8_t levels);

///
/// variables
///

int ledPin = 13; //D2
int recBtnPin = 12; //D4
//bool isRecording = false; //if recording is armed
bool blnRecording = false;
bool stateChanged = false; //if the button state already changed
bool btnState = false;
bool lastBtnState = false;
float currentMillis; //current ms timestamp
float btnPressMillis; //ms Timestamp when btn was pressed
int debounceMillis = 50; //button debounce time in ms


//WIFI
const char *ssid = SECRET_ssid;
const char *password = SECRET_password;
NetworkServer server(80);


//Audio I2s PCM1808 stuff
AudioInfo info(44100, 2, 32); //samplerate
I2SStream i2sStream;
CsvOutput<int32_t> csvOutput(Serial);
StreamCopy copier(csvOutput, i2sStream); //Copy audiostream to CSVoutput
StreamCopy audioStreamToFile;
int i2s_bck = 15; //D15 (Pin 15) BCK (Bit Clock)
int i2s_ws = 16; //RX2 (Pin 16) WS (LRC = left right clock)
int i2s_data = 17; //TX2 (Pin 17) Data (out)
int i2s_mck = 3; //RX0 (Pin 3) MCLK (SCK)

//SDCard Stuff
const char *file_prefix = "/aufnahme_"; //prefix of rec file
const char *file_suffix = ".raw"; //suffix of rec file
char nextRecFileName[64]; //full name of next file to be recorded
File recFile; //Stream that will be saved to SD Card
int fileCount = 0; //how many files are created in current recording Session

const char* dirPrefix = "/Session_"; //prefix of the directory name
int dirCount = 0; //counts how many directories there are, to create next directory
char nextDirName[20]; //name of the next directory to be created
//functions

/// starts SD card recording
void record_start(){
  Serial.println("recording...");
  setNextRecordingName(SD, nextDirName, 0);

  //open output file in Write mode
  recFile = SD.open(nextRecFileName, FILE_WRITE);
  audioStreamToFile.begin(recFile, i2sStream);
  blnRecording = true;
  digitalWrite(ledPin, blnRecording);
}

/// stops SD card recording
void record_end(){
    Serial.println("recording end");
    recFile.close();
    blnRecording = false;
    digitalWrite(ledPin, blnRecording);
}

/// flashes the LED with an pattern
/// expects integer with the error number
/// code 0 = Card Mount Failed
/// code 1 = No SD card attached
/// code 2 = Failed to open directory
/// code 3 = Not a directory
/// code 4 = mkdir failed
/// code 5 = rmdir failed
/// code 6 = Failed to open file for reading
/// code 7 = Failed to open file for writing
/// code 8 = Write failed
/// code 9 = Failed to open file for appending
/// code 10 = Append failed
/// code 11 = Rename failed
/// code 12 = Delete failed
/// code 69 = Success!!
/// code 99 = test error

void error_blink(int errorCode){
  Serial.print("Error ");
  Serial.print(errorCode);
  Serial.print(": ");

  switch (errorCode) {
    case 0:
      Serial.println("Card Mount Failed");
    break;

    case 1:
      Serial.println("No SD card attached");
    break;

    case 2:
      Serial.println("Failed to open directory");
    break;

    case 3:
      Serial.println("Not a directory");
    break;

    case 4:
      Serial.println("mkdir failed");
    break;

    case 5:
      Serial.println("rmdir failed");
    break;

    case 6:
      Serial.println("Failed to open file for reading");
    break;

    case 7:
      Serial.println("Failed to open file for writing");
    break;

    case 8:
      Serial.println("Write failed");
    break;

    case 9:
      Serial.println("Failed to open file for appending");
    break;

    case 10:
      Serial.println("Append failed");
    break;

    case 11:
      Serial.println("Rename failed");
    break;

    case 12:
      Serial.println("Delete failed");
    break;

    case 69:
      Serial.println("SD Card INIT Success!!");
    break;

    case 99:
      Serial.println("Test error");
    break;

    default:
     Serial.println("Unknown error!");
    break;
  }
}

/// reads if button was pressed
/// triggers needed actions
void readBtn() {
  btnState = digitalRead(recBtnPin);
  
  if (btnState != lastBtnState){
    if((millis() - btnPressMillis) > debounceMillis){
      //Serial.println("changed");
      btnPressMillis = millis();

      if (btnState && !lastBtnState) {
        //Serial.println("Press");
        //stateChanged = true;
      }

      if (!btnState && lastBtnState) {
        Serial.println("Release");
        stateChanged = true;
      }
      lastBtnState = btnState;
    }
  }
}

void startNetwork(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loopNetwork(){
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(ledPin, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(ledPin, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void setup() {

  //LED Pin Modes
  pinMode(ledPin, OUTPUT);
  pinMode(recBtnPin, INPUT_PULLUP);

  Serial.begin(115200);
  while(!Serial); // wait for serial to be ready
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);

  auto cfg = i2sStream.defaultConfig(RX_MODE);
    cfg.copyFrom(info);
    cfg.pin_bck = i2s_bck;
    cfg.pin_ws = i2s_ws;
    cfg.pin_data = i2s_data;
    cfg.pin_mck = i2s_mck; // optional master clock pin


    cfg.i2s_format = I2S_STD_FORMAT; // default format
    cfg.is_master = true; // default esp32 is master
    cfg.use_apll = false; //try with true

  //Open SD drive
  sd_setup();

  i2sStream.begin(cfg);
  csvOutput.begin(info);

  error_blink(69); //show success message!!
  lastBtnState = digitalRead(recBtnPin);
  //startNetwork();

  createSessionDir(SD, "/", 0); //count all directories to determine in which dir to save next
}

void loop() {
  currentMillis = millis(); //store current ms timestamp
  //copier.copy(); //copy audio to CSV stream for display on plotter

  //write audio stream to SD card if recording is armed
  if (blnRecording) {
    audioStreamToFile.copy();
  }

  //read if a button has been pressed
  //only read button after debounce time has passed
  readBtn();



  // if button state changed start or stop recording
  if (stateChanged) {
    if (!blnRecording){ //inverted as we want to start when unarmed and stop when armed
      record_start();
    } else {
      record_end();
    }
    
    stateChanged = false;
  }
  
  
  //loopNetwork();
}