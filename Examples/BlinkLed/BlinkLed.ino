#include <neopixel_i2c.h>

neopixel_I2C neoI2C(0x40);

void setup() {
  Serial.begin(57600);

  neoI2C.setup(true);
  neoI2C.setWaitMode(true);
}

void loop() {
  neoI2C.setPixelColor( 0, 0xFF,0xFF,0xFF);
  neoI2C.show();
  delay(300);
  neoI2C.setPixelColor( 0, 0x00,0x00,0x00);
  neoI2C.show();
  delay(300);
}