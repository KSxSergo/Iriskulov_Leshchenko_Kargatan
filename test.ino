//#include <SoftwareSerial.h>

#define  LED LED_BUILTIN
#define CONNECTED 46

void setup()  {
  pinMode(LED, OUTPUT);
  pinMode(CONNECTED, INPUT_PULLUP);
  Serial.begin(38400);
  Serial1.begin(38400);
}

void loop() {
  //if (digitalRead(CONNECTED)) { digitalWrite(LED, HIGH); }
  //else { digitalWrite(LED, LOW); }


  if (Serial1.available()) {
    char c = Serial1.read();
    Serial.print(c);
    if (c == '1') {
        digitalWrite(LED, HIGH);
    } 
    if (c == '0') {
        digitalWrite(LED, LOW);
    }
  }
  if (Serial.available()) {
    char c = Serial.read();
    Serial.print(c);
    Serial1.write(c);
  } 
}