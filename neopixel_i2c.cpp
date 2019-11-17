#include "neopixel_i2c.h"

#define DEFAULT_PIXEL_COUNT  1

neopixel_i2c::neopixel_i2c(void) 
  : I2C_ADR (0), pixelCount (DEFAULT_PIXEL_COUNT)  { 
};

neopixel_i2c::neopixel_i2c(uint8_t I2C_ADR) 
  : I2C_ADR(I2C_ADR), pixelCount (DEFAULT_PIXEL_COUNT) {
};

neopixel_i2c::neopixel_i2c(uint8_t I2C_ADR , uint8_t pixelCount) 
	: I2C_ADR(I2C_ADR), pixelCount(pixelCount) {
};

void neopixel_i2c::setup(void) {
	Wire.begin();
}

void neopixel_i2c::clear(bool show){
	cRGB color = {0x00,0x00,0x00};
	for ( uint8_t i=0; i<pixelCount; i++) {
		setPixelColor(i, &color) ;
	}
	//if(show) this->show();    
}

void     neopixel_i2c::setPixelCount(uint8_t pixelCount) {
  this->pixelCount = pixelCount;
}
uint8_t  neopixel_i2c::getPixelCount(void) {
  return pixelCount;
}


void neopixel_i2c::setI2C_ADR(uint8_t I2C_ADR) { 
  this->I2C_ADR = I2C_ADR;
};


void neopixel_i2c::show(void) {
  Wire.beginTransmission(I2C_ADR);
  Wire.write(REG_CTRL);  
  Wire.write(CTRL_WAIT | CTRL_SET);   // to to: what is with other bits ??
  Wire.endTransmission();
}

void neopixel_i2c::setPixelColor(uint8_t ID, uint8_t r, uint8_t g, uint8_t b) {
  cRGB col = {r,g,b};
  setPixelColor(ID,&col);
}

void neopixel_i2c::setPixelColor(uint8_t ID, cRGB color) {
	setPixelColor(ID, &color);
}

//#include <HardwareSerial.h>
//extern HardwareSerial Serial;

void neopixel_i2c::setPixelColor(uint8_t ID, cRGB* color) {
  //Serial.print  ("                         ");
  //Serial.print  (ID);
  //Serial.print  (" : ");
  //Serial.println(ID*3+REG_FIRSTLED);
  if(ID>83) return;
  
  Wire.beginTransmission(I2C_ADR);
  //uint16_t adr = ID; adr *= 3; adr +=REG_FIRSTLED;
  //Wire.write(adr);  
  Wire.write(ID*3+REG_FIRSTLED);  
  //Wire.write((uint8_t*) color,3);  
  Wire.write(color->b);  
  Wire.write(color->g);  
  Wire.write(color->r);  
  Wire.endTransmission();
}

void neopixel_i2c::setWaitMode(bool waitForShow) {
  Wire.beginTransmission(I2C_ADR);
  Wire.write(REG_CTRL);  
  Wire.write((waitForShow?CTRL_WAIT:0));  
  Wire.endTransmission();
}
