#include "neopixel_i2c.h"
#include <Arduino.h>

/*****************************************************************/
/* Global register addresses                                     */
/*****************************************************************/
#include "slave/i2c_slave_defs.h"

/*****************************************************************/
/* Construction and setup of the class                           */
/*****************************************************************/

neopixel_i2c::neopixel_i2c(uint8_t I2C_ADR) 
  : I2C_ADR(I2C_ADR) {};

#ifdef NEOPIXEL_RGBW_SUPPORT
void neopixel_i2c::setRGBWMode(bool RGBW=true) {
}
#endif   

bool neopixel_i2c::setup(uint8_t pixelCount, bool waitForShow) {	
	Wire.begin();
	reset();
    if(waitForShow) this->setWaitMode();
	return setPixelCount(pixelCount);
}

void neopixel_i2c::reset(void) {
  uint8_t reg = 0;
  SETBIT(reg,CTRL0_RST);
  setRegister(REG_CTRL0,reg);
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
	setRegister(REG_LED_CNT,pixelCount);
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
  uint8_t reg = getRegister(REG_CTRL0);
  if (waitForShow)  SETBIT(reg , CTRL0_WAIT);
  else				CLRBIT(reg , CTRL0_WAIT);
  setRegister(REG_CTRL0, reg);
}
void neopixel_i2c::show(void) {
  uint8_t reg = getRegister(REG_CTRL0);
  SETBIT( reg, CTRL0_SHOW);
  setRegister(REG_CTRL0, reg);
}

uint8_t neopixel_i2c::getVersion(void) {
  return getRegister(REG_VER);
}

/*****************************************************************/
/* Generic reading and setting of registers                      */
/*****************************************************************/

void neopixel_i2c::setRegisters(uint8_t adr, uint8_t value0,uint8_t value1,uint8_t value2) {
  Wire.beginTransmission(I2C_ADR);
  Wire.write(adr);  
  Wire.write(value0);  
  Wire.write(value1);  
  Wire.write(value2);  
  Wire.endTransmission();
}
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
  //bool scaleAdr = (adr > 9); // can be used to test scaling
  
  // check for maximum address
  if ( adr > (256*3-1) ) return;

  uint8_t reg = getRegister(REG_CTRL0);
  
  // if address would be higher than 255, set scaling address
  if( scaleAdr ) { 
    CLRBIT(reg, CTRL0_M0); 
	SETBIT(reg, CTRL0_M1); 
    setRegister(REG_CTRL0, reg);
	adr = adr/3;
  }	
  
  setRegisters(adr,b,r,g);
	
  // clear scaling bits
  if( scaleAdr ) { 
	CLRBIT(reg, CTRL0_M1); 
	CLRBIT(reg, CTRL0_M0); 
    setRegister(REG_CTRL0, reg);
  }
}

void neopixel_i2c::setPixelColorGlobal(cRGB color, bool show) {
  setPixelColorGlobal(color.r,color.g,color.b,show);
}
void neopixel_i2c::setPixelColorGlobal(uint8_t r, uint8_t g, uint8_t b, bool show){
  setRegisters(REG_GLB_G,b,r,g);
  uint8_t reg = getRegister(REG_CTRL0);
  SETBIT( reg, CTRL0_SETG);
  setRegister(REG_CTRL0,reg);
  if(show) this->show();  
}
	
void neopixel_i2c::clear(bool show){
  //for ( uint8_t i=0; i<pixelCount; i++) setPixelColor(i, 0x00,0x00,0x00);
  setPixelColorGlobal(0x00,0x00,0x00);
  if(show) this->show();    
}	

/*****************************************************************/
/* Configure, set and read GPIO                                  */
/*****************************************************************/

void    neopixel_i2c::configGPIO(uint8_t config) {
	uint8_t reg = getRegister(REG_CTRL1);
	
	
	// ToDo: change to registers in i2c_slave_defs defines
	switch (config & 0x03) {
		case GPIOa_OUTPUT:		 reg = (reg & ~B000011) | B10; break;
		case GPIOa_INPUT:		 reg = (reg & ~B000011) | B00; break;
		case GPIOa_INPUT_PULLUP: reg = (reg & ~B000011) | B01; break;
	}

	switch (config & 0x30) {
		case GPIOb_OUTPUT:	 	 reg = (reg & ~B11000) | B10000; break;
		case GPIOb_INPUT:		 reg = (reg & ~B11000) | B00000; break;
		case GPIOb_INPUT_PULLUP: reg = (reg & ~B11000) | B01000; break;
	}
	
	setRegister(REG_CTRL1,  reg);
}


void neopixel_i2c::setGPIO(uint8_t level) {
	uint8_t reg = getRegister(REG_CTRL1);
	
	if( (level & 0x03) == GPIOa_LOW)         reg = reg & ~B000100;
	else if( (level & 0x03) == GPIOa_HIGH) 	 reg = reg |  B000100;

	if( (level & 0x30) == GPIOb_LOW)         reg = reg & ~B100000;
	else if( (level & 0x30) == GPIOb_HIGH) 	 reg = reg |  B100000;

	setRegister(REG_CTRL1,  reg);
}

uint8_t neopixel_i2c::getGPIO(void) {
	uint8_t ret = 0;
	uint8_t reg = getRegister(REG_CTRL1);
	
	if ( ISBITSET(reg,CTRL1_IOa2) ) ret |= GPIOa_HIGH;
	else						    ret |= GPIOa_LOW;
	
	if ( ISBITSET(reg,CTRL1_IOb2) ) ret |= GPIOb_HIGH;
	else						    ret |= GPIOb_LOW;
	
	return  ret;
}
	

