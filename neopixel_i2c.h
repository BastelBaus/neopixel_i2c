#ifndef __NEOPIXEL_I2C_H__
#define __NEOPIXEL_I2C_H__

// set to zero to exclude GPIO handling
#define NEOPIXEL_USE_GPIO		1
//#undef  NEOPIXEL_USE_GPIO

// defined to add RGBW support (currently not implemented)
//#define NEOPIXEL_RGBW_SUPPORT   1
//#undef NEOPIXEL_RGBW_SUPPORT

#include <Wire.h>
#include "neopixel_i2c.h"
#include "cRGB.h"
#ifdef NEOPIXEL_RGBW_SUPPORT
	#include "cRGBW.h"
#endif

class neopixel_i2c {
  public:

    // constructor
    neopixel_i2c(uint8_t I2C_ADR);
	
	// Initializes and setups the library. Returns true if setup was sucessfully, else false (i.e. pixel count above possible pixel count).
	// - In default mode, a pixel is direly updated if the color changes.
	// - can be called again and can also be used to change the pixel count dynamically
//#ifndef NEOPIXEL_RGBW_SUPPORT
	bool setup(uint8_t pixelCount=1, bool waitForShow=false);
//#else
//	bool setup(uint8_t pixelCount=1, bool waitForShow=false, bool RBGWMode = false);
//	void setRGBWMode(bool RGBW=true);   
//#endif   

    // change the Mode. waitForShow 
	//     true: only updates the pixels from RAM
	//
	void setWaitMode(bool waitForShow=true);   
    void show(void);      // show all curent lights / settings // sets also the wait mode active
    void clear(bool show = true);     // set all pixels off, if show is = false, only in RAM



    // Sets the count of used pixels. Returns true if successfull, else false 
    // and nothing is changed.
	bool     setPixelCount(uint8_t pixelCount);
    // Returns the currently set pixel count.
	uint8_t  getPixelCount(void);
	// Returns the maximum supported pixel count for a mode from the slave 
	// device. Note that the setup function has to be run before.
//#ifndef NEOPIXEL_RGBW_SUPPORT
    uint8_t  getMaxPixelCount();
//#else
//    uint8_t  getMaxPixelCount(bool RGBW = false);
//#endif   

	// Set pixel colors. 
    void setPixelColor(uint8_t ID, cRGB color); // set color of a light but does not show it
    void setPixelColor(uint8_t ID, uint8_t r, uint8_t g, uint8_t b); // set color of a light but does not show it
//#ifdef NEOPIXEL_RGBW_SUPPORT
    //void setPixelColor(uint8_t ID, cRGBW color); // set color of a light but does not show it
	//void setPixelColor(uint8_t ID, uint8_t r, uint8_t g, uint8_t b, uint8_t w); // set color of a light but does not show it
//#endif

#ifdef NEOPIXEL_USE_GPIO
    // Configure, set and get GPIOs
	void    configGPIO(uint8_t config);
	void    setGPIO0(uint8_t level);
	void    setGPIO1(uint8_t level);
	uint8_t setGPIO0(void);
	uint8_t setGPIO1(void);
#endif

  protected:;
  public:;
	// sets teh hiogh bit off the address (A9 & A8) and returns the remaining 8bit address
	uint8_t setAddress(uint16_t adr16);
	void 	setRegister(uint8_t adr, uint8_t value);
	uint8_t getRegister(uint8_t adr);
	void reset(void);      // turn all lights off and reset registers. "All means MAX lights fingfigued in slave. ALso cleas the register counts
    
  private:
    uint8_t I2C_ADR;
    uint8_t pixelCount;
#ifdef NEOPIXEL_RGBW_SUPPORT
	uint8_t RBGWMode;
#endif

}; // class neopixel_i2c {




#endif
