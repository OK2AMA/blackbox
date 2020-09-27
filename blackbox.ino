/*
   HW: ESP32 DO it DEV KIT 1
   LCD 5110, OLED 1306
   BMP280, GPS receiver
   rotary encoder

   pinout:
   LCD 5110 / GPIO
   RST   D23
   CE    D1/txd
   Dout  D19
   Din   D18
   CLK   D5
   BCKL  D2

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

SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h
Adafruit_BMP280 bmp; // I2C


void setup() {
  Serial.begin(115200);
  Serial.println();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

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

}

void loop() {
  drawFontFaceDemo();
  // write the buffer to the display
  while ( true ) {
  
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Teplota");
    display.drawString(50, 0, String(bmp.readTemperature()) + "*C");
    display.drawString(0, 15, "Tlak");
    display.drawString(50, 15, String(bmp.readPressure()) + "pA");
    display.display();

   delay(2000);
   display.clear();

  }
}
