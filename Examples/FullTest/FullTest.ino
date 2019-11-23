/******************************************************************************/
/* Small example to show all functionalities of teh neopixel_i2c library      */
/******************************************************************************/

#include <neopixel_i2c.h>
#include <Wire.h>

#define LED_COUNT        255
#define I2C_ADDRESS      0x40

#define WAIT_FOR_UPDATE  true

neopixel_i2c neoI2C( I2C_ADDRESS );
uint8_t pixCount;

void setup() {
  Serial.begin(57600);

  // try to set up LED_COUNT of LEDs or fall back to maximum supported count
  Serial.print("Initializing neopixel_i2c with ");  
  Serial.print(LED_COUNT);  
  Serial.println(" LEDs");  
  if (!neoI2C.setup(LED_COUNT,WAIT_FOR_UPDATE) ) {
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
  
  pixCount = neoI2C.getPixelCount();
  neoI2C.configGPIO(GPIOa_OUTPUT|GPIOb_OUTPUT);        // TOGGLE IOs


  Serial.print  ( "neopixel v 0x");
  Serial.print  ( neoI2C.getVersion(),HEX); 
  Serial.print  ( " initialized with " );
  Serial.print  ( pixCount  );
  Serial.println( " LEDs" );

  // change the I2C speed to get faster data trasmission
  Wire.setClock(400000);
  
  
}

uint8_t offset = 0;
void loop() {
  Serial.print( "Loop: " ); Serial.print( offset ); Serial.print( " of " );
  Serial.println( pixCount );
  
  neoI2C.clear(false);                           // clear RAM but do not update LEDs
  neoI2C.setPixelColor( offset, 0x00,0x00,0xFF); // set a pixel to a rainbow color
  neoI2C.show();                                 // update all pixels just now
//  neoI2C.setGPIO(GPIOa_LOW | GPIOb_HIGH);        // TOGGLE IOs
  delay(400);
    
  uint16_t HUE = (uint16_t) offset * (MAX_HUE / pixCount); 
  cRGB col = getRGBfromHSV(HUE, 0xFF,0xFF);       // get RGB from HSW value
  neoI2C.setPixelColorGlobal(col,false);          // set all pixels in RAM to blue
  neoI2C.setPixelColor( offset, 0xFF,0x00,0x00 ); // set a pixel to white
  neoI2C.show();                                  // update all pixels just now
  //neoI2C.setGPIO(GPIOb_LOW | GPIOa_HIGH);         // TOGGLE IOs
  delay(400);
  
  offset++;
  if( offset >= pixCount) offset = 0;
}