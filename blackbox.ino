/*
   HW: ESP32 DO it DEV KIT 1
   LCD 5110, OLED 1306
   BMP280, GPS receiver
   rotary encoder

   pinout:
   LCD 5110 / GPIO
   1 RST   D23
   2 CE    D1/txd
   3 DC    D19
   4 DIN   D18
   5 CLK   D5
   6 VCC
   7 DL    D2
   8 GND

   Rotary encoder
   left  D17
   right D4
   buttn D15

   I2C: OLED,BME280
   SDA  D21
   SCL  D22
*/

#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "WiFi.h"

const char* ssid = "OpenWrt";
const char* password =  "fialkapodlampou";

Adafruit_PCD8544 displayLCD = Adafruit_PCD8544(5, 18, 19, 32, 23);
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h
Adafruit_BMP280 bmp; // I2C
TinyGPS gps;
HardwareSerial hwSerial_2(2);

#define RX2 16
#define TX2 17
#define bulh_const (3.3*((15+20)/20))/4096

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  displayLCD.begin();
  displayLCD.setRotation(2);
  displayLCD.setContrast(55);
  displayLCD.display(); // show splashscreen
  displayLCD.clearDisplay();   // clears the screen and buffer
  displayLCD.drawPixel(0, 0, BLACK);
  displayLCD.display();
  displayLCD.setTextSize(1);
  displayLCD.setTextColor(BLACK);
  displayLCD.setCursor(0, 0);
  displayLCD.println("Hello, world!");
  displayLCD.clearDisplay();
  displayLCD.display();

  pinMode(2, OUTPUT);    // backlight
  digitalWrite(2, HIGH); // sets the digital pin 13 on

  hwSerial_2.begin(9600, SERIAL_8N1, RX2, TX2);

  bmp.begin(0x76);
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void drawFontFaceDemo() {
  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");
  display.display();
  display.clear();


}

void loop() {
  bool newData = false;
  unsigned long chars, age, date = 0;
  unsigned short sentences, failed;

  drawFontFaceDemo();
  // write the buffer to the display
  while ( true ) {

    Serial.println();

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Teplota");
    display.drawString(45, 0, String(bmp.readTemperature()) + " *C");
    display.drawString(0, 15, "Tlak");
    display.drawString(45, 15, String(bmp.readPressure() / 100) + " hPa");
    display.drawString(0, 30, "Napeti: " + String(analogRead(13) * 0.00141)  + " V");
    
    display.display();

    delay(1);

    // For one second we parse GPS data and report some key values
    for (unsigned long start = millis(); millis() - start < 1000;)
    {
      while (hwSerial_2.available())
      {
        char c = hwSerial_2.read();
        Serial.print(c);

        if (gps.encode(c)) // Did a new valid sentekince come in?
          newData = true;
      }
    }
    if (newData)
    {
      float flat, flon;
      unsigned long age;
      gps.f_get_position(&flat, &flon, &age);
      Serial.print("LAT=");
      Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
      Serial.print(" LON=");
      Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
      Serial.print(" SAT=");
      Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
      Serial.print(" PREC=");
      Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());

      displayLCD.setCursor(0, 0);
      displayLCD.println("GPS receiver:");
      displayLCD.print("La:");
      displayLCD.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 5);
      displayLCD.print("Lo:");
      displayLCD.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 5);
      displayLCD.print("ST=");
      displayLCD.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
      displayLCD.print(" AL=");
      displayLCD.println(gps.f_altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_altitude(),1);
      displayLCD.print("SPD=");
      displayLCD.print(gps.f_speed_kmph() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_speed_kmph(),1);
      displayLCD.print("SPD=");
      displayLCD.print(gps.f_speed_kmph() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_speed_kmph(),1);



      displayLCD.display();
      displayLCD.clearDisplay();

    }
    else
    {
      displayLCD.setCursor(20, 20);
      displayLCD.println("No GPS fix");
      displayLCD.display();
      displayLCD.clearDisplay();
    }

    gps.stats(&chars, &sentences, &failed);
    Serial.print(" CHARS=");
    Serial.print(chars);
    Serial.print(" SENTENCES=");
    Serial.print(sentences);
    Serial.print(" CSUM ERR=");
    Serial.println(failed);
    if (chars == 0)
      Serial.println("** No characters received from GPS: check wiring **");

    display.clear();

  }
}
