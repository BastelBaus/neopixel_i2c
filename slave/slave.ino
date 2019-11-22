/*
* Neopixel I2C Slave application
*
* At boot, scrolls a bright spot (init_color) along the array
* Will stop as soon as an i2c transaction is received
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "i2c_slave_defs.h"
#include "i2c_machine.h"
#include "light_ws2812.h"

#define CODE_VERSION  0x01

#define GPIOa   	  PB1
#define GPIOb   	  PB4

volatile uint8_t i2c_reg[I2C_N_REG];

//static inline void set_leds_global(void) {
//  ws2812_setleds_constant((struct cRGB *)&i2c_reg[REG_GLB_G], N_LEDS);
//}

static inline void update_leds(void) {
  ws2812_sendarray((uint8_t *) i2c_reg + I2C_N_GLB_REG, N_LEDS * LED_COLS);
  CLRBIT(REG_CTRL0,CTRL0_SHOW);   
}

void initialize(void) {

  cli();
  
  // clear all LEDs 
  // initialize all control registers
  SETREG(REG_CTRL0,0);
  SETREG(REG_CTRL1,0);
  SETREG(REG_LED_CNT,0);
  SETREG(REG_MAX_LED,N_LEDS);
  SETREG(REG_GLB_G,0);
  SETREG(REG_GLB_R,0);
  SETREG(REG_GLB_B,0);
  SETREG(REG_GLB_W,0);  
  SETREG(REG_VER,CODE_VERSION);
  ws2812_setleds_constant((struct cRGB *)&i2c_reg[REG_GLB_G], N_LEDS);
  
  // Reset the LED registers to zero 
  volatile uint8_t *p = i2c_reg + I2C_N_GLB_REG;
  uint16_t i = I2C_N_GLB_REG + N_LEDS*LED_COLS;
  while (i--) { *p = 0; p++; }
  // todo: replace with memset
  
  handleIOs();
  sei();
}

void fillRAMRegistersWithGlobal(void) {
  volatile uint8_t *p = i2c_reg + I2C_N_GLB_REG;
  uint16_t i = I2C_N_GLB_REG + N_LEDS*LED_COLS;
  uint8_t k=2;
  while (i--) { 
	switch(k) {
	  case 0: *p = REG_GLB_G;  break;
	  case 1: *p = REG_GLB_R;  break;
	  case 2: *p = REG_GLB_B;  break;
	}
	if(k>0) k--; else k=2;
	 
	p++; 
  }
}



static inline void handleIOs(void) {

  // set or read IOs
  if( ! ISBITSET(REG_CTRL1,CTRL1_IOa1) ) { 
    pinMode(GPIOa,( ISBITSET(REG_CTRL1,CTRL1_IOa0) ? INPUT_PULLUP : INPUT ) ); 
  } else if (! ISBITSET(REG_CTRL1,CTRL1_IOa0) ) {
    pinMode(GPIOa,OUTPUT ); 
  } else {} //reserved

  if( ! ISBITSET(REG_CTRL1,CTRL1_IOb1) ) { 
    pinMode(GPIOb,( ISBITSET(REG_CTRL1,CTRL1_IOb0) ? INPUT_PULLUP : INPUT ) ); 
  } else if (! ISBITSET(REG_CTRL1,CTRL1_IOb0) ) {
    pinMode(GPIOb,OUTPUT ); 
  } else {} //reserved


	// set or read IOs
	if( ! ISBITSET(REG_CTRL1,CTRL1_IOa1) ) { 
		if( digitalRead(GPIOa) ) SETBIT(REG_CTRL1,CTRL1_IOa2); 
		else                     CLRBIT(REG_CTRL1,CTRL1_IOa2); 
	} else if (! ISBITSET(REG_CTRL1,CTRL1_IOa0) ) {
	  digitalWrite(GPIOa,(ISBITSET(REG_CTRL1,CTRL1_IOa2)?HIGH:LOW) );
  } else {} //reserved

	if( ! ISBITSET(REG_CTRL1,CTRL1_IOb1) ) { 
		if( digitalRead(GPIOb) ) SETBIT(REG_CTRL1,CTRL1_IOb2); 
		else                     CLRBIT(REG_CTRL1,CTRL1_IOb2); 
  } else if (! ISBITSET(REG_CTRL1,CTRL1_IOb0) ) {
    digitalWrite(GPIOb, (ISBITSET(REG_CTRL1,CTRL1_IOb2)?HIGH:LOW) );
  } else {} //reserved

}


void setup(void) {
  DDRB = (1 << 3);
  i2c_init();
  initialize();
}

//ToDo: switch RGBW mode

void loop(void) {

	handleIOs();
 
	if (i2c_check_stop()) {
		// todo: only if relevant registers are updated!

    // reset the LEDs and slave configuration
    if ( ISBITSET(REG_CTRL0,CTRL0_RST) ) {
      initialize();
      return;
    }

    // fill RAM with global LED value
	  if( ISBITSET(REG_CTRL0,CTRL0_SETG) ) {
		  fillRAMRegistersWithGlobal();
		  CLRBIT(REG_CTRL0,CTRL0_SETG);
	  }
	  	  
	  // show leds
	  if ( ISBITSET(REG_CTRL0,CTRL0_WAIT) )   {
		  if ISBITSET(REG_CTRL0,CTRL0_SHOW) update_leds();		    
		}	else update_leds();
    
	  
	} // if (i2c_check_stop()) {
}