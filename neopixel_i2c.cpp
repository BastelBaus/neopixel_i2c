#include "neopixel_i2c.h"
#include <Arduino.h>

/*****************************************************************/
/* Global register addresses                                     */
/*****************************************************************/
#define REG_CTRL    0
  #define CTRL_RST    (1 << 0) // 
  #define CTRL_GLB    (1 << 1)
  #define CTRL_WAIT   (1 << 2) // 0: update each pixel after each command. 1: only updates pixels if CTRL_SHOW set is set
  #define CTRL_SHOW   (1 << 3) // 1: all LEDs are set, after finish, value is reset to 0
  #define CTRL_M0     (1 << 6) // M1:M0 = B00 ==> k=1   // M1:M0 = B01 ==> k=2
  #define CTRL_M1     (1 << 7) // M1:M0 = B10 ==> k=3   // M1:M0 = B00 ==> k=4
  #define GET_CTRL_k  ( ( (REG_CTRL &  (CTRL_M1|CTRL_M0)) >>6 )  + 1 )

#define REG_LED_CNT    1
#define REG_MAX_LED    2
#define REG_GLB_G      3
#define REG_GLB_R      4
#define REG_GLB_B      5
#ifdef NEOPIXEL_USE_GPIO
#define REG_GPIO_CTRL  6
   // Configure, set and get GPIOs
  #define REG_GPIO0_CTRL    0
  #define REG_GPIO0_IO      2
  #define REG_GPIO1_CTRL    3
  #define REG_GPIO1_IO      5 
  #define GPIO_OUTPUT       0
  #define GPIO_INPUT        1
  #define GPIO_INPUT_PULLUP 2
#endif
	
#define REG_FIRSTLED    12

/*****************************************************************/
/* Construction and setup of the class                           */
/*****************************************************************/

neopixel_i2c::neopixel_i2c(uint8_t I2C_ADR) 
  : I2C_ADR(I2C_ADR) {};

#ifdef NEOPIXEL_RGBW_SUPPORT
bool neopixel_i2c::setup(uint8_t pixelCount, bool waitForShow, bool RBGWMode) {	
}
void neopixel_i2c::setRGBWMode(bool RGBW=true) {
}
#else
bool neopixel_i2c::setup(uint8_t pixelCount, bool waitForShow) {	
	Wire.begin();
	reset();
    if(waitForShow) this->setWaitMode();
	return setPixelCount(pixelCount);
}
#endif   

void neopixel_i2c::reset(void) {
  setRegister(REG_CTRL,CTRL_RST);
}
/*****************************************************************/
/* Utility functionality                                         */
/*****************************************************************/
#ifndef NEOPIXEL_RGBW_SUPPORT
uint8_t  neopixel_i2c::getMaxPixelCount() {
	return getRegister(REG_MAX_LED);
}
#else
uint8_t  neopixel_i2c::getMaxPixelCount(bool RGBW = false) {
	return getRegister(REG_MAX_LED);
}
#endif   

bool neopixel_i2c::setPixelCount(uint8_t pixelCount) {
  if ( pixelCount <=  getMaxPixelCount() ) {
    delay(1);
	setRegister(REG_LED_CNT,pixelCount);
    delay(1);
	this->pixelCount = pixelCount;
	return true;
  }
  return false;
}
uint8_t  neopixel_i2c::getPixelCount(void) {
  //pixelCount = getRegister(REG_LED_CNT);
  return pixelCount;
}

void neopixel_i2c::setWaitMode(bool waitForShow) {
  if (waitForShow)  setRegister(REG_CTRL, getRegister(REG_CTRL) | CTRL_WAIT );
  else				setRegister(REG_CTRL, getRegister(REG_CTRL) & (~CTRL_WAIT) );
}
void neopixel_i2c::show(void) {
  setRegister(REG_CTRL, getRegister(REG_CTRL) | CTRL_SHOW );
}

/*****************************************************************/
/* Generic reading and setting of registers                      */
/*****************************************************************/

void neopixel_i2c::setRegister(uint8_t adr, uint8_t value) {
  Wire.beginTransmission(I2C_ADR);
  Wire.write(adr);  
  Wire.write(value);  
  Wire.endTransmission();
}

uint8_t neopixel_i2c::getRegister(uint8_t adr) {
  uint8_t value = 0;
  Wire.beginTransmission(I2C_ADR);
  Wire.write(adr);  
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADR, 1);
  if( Wire.available() ) {
    value = Wire.read();    // receive a byte as character
  }  
  Wire.endTransmission();
  return value;
}

/*****************************************************************/
/* Setting pixel colors                                          */
/*****************************************************************/

//#include <HardwareSerial.h>
//extern HardwareSerial Serial;

void neopixel_i2c::setPixelColor(uint8_t ID, cRGB color) {
  setPixelColor(ID,color.r,color.g,color.b);
}

void neopixel_i2c::setPixelColor(uint8_t ID, uint8_t r, uint8_t g, uint8_t b) {
  uint16_t adr = ID*3 + REG_FIRSTLED;
  bool scaleAdr = (adr > 255);
  //bool scaleAdr = (adr > 9);

  // check for maximum address
  if ( adr > (256*3-1) ) return;
  
  // if address would be higher than 255, set scaling address
  if( scaleAdr ) {
    //setRegister(REG_CTRL, (getRegister(REG_CTRL) & (~CTRL_M0) ) | CTRL_M1 );
	//adr /= 3;
  }	
  
  Wire.beginTransmission(I2C_ADR);
  Wire.write(adr);  
  Wire.write(b);  
  Wire.write(g);  
  Wire.write(r);  
  Wire.endTransmission();
	
  // clear scaling bits
  if( scaleAdr ) {
	//setRegister(REG_CTRL, (getRegister(REG_CTRL) & (~CTRL_M0) ) & (~CTRL_M1) );
  }
}

void neopixel_i2c::clear(bool show){
  for ( uint8_t i=0; i<pixelCount; i++) {
    setPixelColor(i, 0x00,0x00,0x00) ;
  }
  //if(show) this->show();    
}	

/*****************************************************************/
/* Configure, set and read GPIO                                  */
/*****************************************************************/

#ifdef NEOPIXEL_USE_GPIO
void neopixel_i2c::configGPIO(uint8_t config) {
}
void neopixel_i2c::setGPIO0(uint8_t level) {
}
void neopixel_i2c::setGPIO1(uint8_t level) {
}
uint8_t neopixel_i2c::setGPIO0(void) {
}
uint8_t neopixel_i2c::setGPIO1(void){
}

#endif
