//HC-05 Setup

String INFO = "Enter AT commands:\n\
AT            Check AT mode\n\
AT+NAME?      Get HC-05 name\n\
AT+NAME=...   Set HC-05 name\n\
AT+PSWD?      Get HC-05 password\n\
AT+PSWD=...   Set HC-05 password\n\
AT+ADDR?      Get HC-05 address\n\
AT+ROLE?      Get HC-05 role\n\
AT+ROLE=0     Set HC-05 Slave role\n\
AT+ROLE=1     Set HC-05 Master role\n\
AT+BIND=...*  Establish a fixed relationship\n\
AT+BIND?      Get address of ^^^\n\
*XXXX,XX,XXXXXX\n\
";

void setup() {
  Serial.begin(38400);  //USB //9600
  Serial1.begin(38400); //Bluetooth
  Serial.println(INFO);
}

void loop() {
  if (Serial.available())  { Serial1.write(Serial.read()); }
  if (Serial1.available()) { Serial.write(Serial1.read()); }
}