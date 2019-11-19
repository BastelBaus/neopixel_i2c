#include <neopixel_i2c.h>

#define LED_COUNT    255
#define I2C_ADDRESS  0x40

neopixel_i2c neoI2C( I2C_ADDRESS );

void setup() {
  Serial.begin(57600);

  // try to set up LED_COUNT of LEDs or fall back to maximum supported count
  Serial.print("Initializing neopixel_i2c with ");  
  Serial.print(LED_COUNT);  
  Serial.println(" LEDs");  
  if (!neoI2C.setup(LED_COUNT,true) ) {
	  uint8_t maxPixels = neoI2C.getMaxPixelCount();
	  Serial.print  (LED_COUNT);
	  Serial.print  (" LEDs not supported, try maximum amount of ");
	  Serial.print  (maxPixels);  
    Serial.println(" LEDs");  
    if ( !neoI2C.setPixelCount(maxPixels) ) {
		  Serial.println("Initialization failed. Haning up !!");
		  while(1) {}
	  }
  } // if (!neoI2C.setup(LED_COUNT,true) ) {

  Serial.print  ( "neopixel initialized with " );
  Serial.print  ( neoI2C.getPixelCount() );
  Serial.println( " LEDs" );
}

uint8_t offset = 0;
void loop() {

  neoI2C.clear(false);
  neoI2C.setPixelColor( offset, 0xFF,0xFF,0xFF);
  neoI2C.show();
  delay(500);
  
  offset++;
  if( offset >= neoI2C.getPixelCount()) offset = 0;
}