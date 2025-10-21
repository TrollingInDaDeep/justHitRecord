// ESP32 Code to record audio from jack socket and save to SD Card

///
/// variables
///
int ledPin = 18;
int btnPin = 19;
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