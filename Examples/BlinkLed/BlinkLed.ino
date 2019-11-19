#include <neopixel_i2c.h>

#define LED_COUNT    2
#define I2C_ADDRESS  0x40

neopixel_i2c neoI2C( I2C_ADDRESS );

void setup() {
  neoI2C.setup(LED_COUNT); 
}


void loop() {

  for(uint8_t i = 0; i < neoI2C.getPixelCount() ; i++)
    neoI2C.setPixelColor( i, 0xFF,0xFF,0xFF);
  delay(300);
  
  neoI2C.clear();
  delay(300);
}