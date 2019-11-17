

#ifndef __NEOPIXEL_I2C_H__
#define __NEOPIXEL_I2C_H__

#include <Wire.h>
#include "cRGB.h"

class neopixel_i2c {
  public:
    neopixel_i2c(void);
    neopixel_i2c(uint8_t I2C_ADR);
    neopixel_i2c(uint8_t I2C_ADR, uint8_t pixelCount);
	
	void setup(void);
	void setI2C_ADR(uint8_t I2C_ADR);
    
    void     setPixelCount(uint8_t pixelCount);
    uint8_t  getPixelCount(void);

    void setPixelColor(uint8_t ID, cRGB color); // set color of a light but does not show it
    void setPixelColor(uint8_t ID, cRGB* color); // set color of a light but does not show it
    void setPixelColor(uint8_t ID, uint8_t r, uint8_t g, uint8_t b); // set color of a light but does not show it

    void setWaitMode(bool waitForShow);   // default: directly show the new LED color
    void show(void);      // show all curent lights / settings // sets also the wait mode active
    void clear(bool show = true);     // set all pixels off, if show is = false, only in RAM
    // ToDO:
    // setCount
    

  private:
    uint8_t I2C_ADR;
    uint8_t pixelCount;

};

// register addresses
#define REG_CTRL    0
#define     CTRL_RST    (1 << 0) // 
#define     CTRL_GLB    (1 << 1)
#define     CTRL_WAIT   (1 << 2) // if 1 only updates LED if CTRL set is set
#define     CTRL_SET    (1 << 3) // if 1, al LEDs are set to array, after this passes, reset to 0

#define REG_GLB_G      1
#define REG_GLB_R      2
#define REG_GLB_B      3
#define REG_FIRSTLED   4


#endif
