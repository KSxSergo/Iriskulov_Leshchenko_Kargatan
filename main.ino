#include <Temperature_LM75_Derived.h> 
#include <DHT22.h> 
#include <Adafruit_BMP280.h> 
#include <LiquidCrystal_I2C.h>

#define BMP_SCK  (9) //scl 
#define BMP_MISO (12) //sdo 
#define BMP_MOSI (10) //sda 
#define BMP_CS   (11) //csb 

#define interval 6000000UL

uint32_t TIME = 0, START_MILLIS = 0, START_TIME = 0;

String get_time() { //get current time as "HH:MM:SS"
  String vremya = "";
  if(TIME / 3600 >= 10) {
    vremya += String(TIME / 3600);
  } else {
    vremya += "0" + String(TIME / 3600);
  }
  if(TIME % 3600 / 60 >= 10) {
    vremya += ":" + String(TIME % 3600 / 60);
  } else {
    vremya += ":0" + String(TIME % 3600 / 60);
  }
  if(TIME % 3600 % 60 >= 10) {
    vremya += ":" + String(TIME % 3600 % 60);
  } else {
    vremya += ":0" + String(TIME % 3600 % 60);
  }
  return vremya;
}

Generic_LM75 temperature; 
DHT22 dht22(5); 
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK); 
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() { 
  while(!Serial) delay(1000); 
  Serial.begin(38400);
  Serial1.begin(38400);
  Wire.begin(); 
  bmp.begin();
  lcd.init();
  lcd.backlight();
  while(true){
    Serial.println("<time>");
    String vremya = Serial.readStringUntil('\n');

    if((vremya[0] == '<') && (vremya[vremya.length() - 1] == '>')) { //set current got time
      START_TIME = vremya.substring(1, vremya.length() - 1).toInt();
      START_MILLIS = millis();
      Serial.println("<ok>");
      break;
    }
  }
} 
 
void loop() {
  TIME = (START_TIME + (millis() - START_MILLIS) / 1000) % 86400;

   if(abs(millis() - START_MILLIS) > interval) {
      Serial.println("<time>"); //time request
      String vremya = Serial.readStringUntil('\n');
      if((vremya[0] == '<') && (vremya[vremya.length() - 1] == '>')) { //set current got time
        START_TIME = vremya.substring(1, vremya.length() - 1).toInt();
        START_MILLIS = millis();
        Serial.println("<ok>");
      }
    }
  
  String s_tmp = String(round(temperature.readTemperatureC() * 10) / 10.);
  String s_hmd = String(round(dht22.getHumidity() * 10) / 10.);
  String s_prs = String(round(bmp.readPressure()*760/101325 * 10) / 10.);

  s_tmp = s_tmp.substring(0, s_tmp.length() - 1);
  s_hmd = s_hmd.substring(0, s_hmd.length() - 1);
  s_prs = s_prs.substring(0, s_prs.length() - 1);

  String vremya = get_time();
  String data = "{'temp': '" + s_tmp + "', 'humid': '" + s_hmd + "', 'press': '" + s_prs + "', 'time': '" + vremya + "'}";

  Serial.println(data);
  Serial1.println(data);
  lcd.setCursor(0, 0);
  //lcd.print(s_tmp + "C  " + s_hmd + "%  ");
  lcd.print(s_tmp + char(223) + "C  " + vremya);
  lcd.setCursor(0, 1);
  //lcd.print(s_prs + "mmHg " + "12" + ":" + "34");
  lcd.print(s_hmd + "%  " + s_prs + "mmHg ");


  delay(1000);

}
