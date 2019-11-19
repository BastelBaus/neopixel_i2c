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

volatile uint8_t i2c_reg[I2C_N_REG];
const uint8_t init_color[LED_COLS] PROGMEM = { 0xFF, 0xFF, 0xFF };

static inline void set_leds_global(void) {
  ws2812_setleds_constant((struct cRGB *)&REG_GLB_G, N_LEDS);
}

static inline void update_leds(void) {
  ws2812_sendarray((uint8_t *) i2c_reg + I2C_N_GLB_REG, N_LEDS * LED_COLS);
}

void initialize(void) {

  cli();
  
  // clear all LEDs 
  REG_GLB_G = 0;
  REG_GLB_R = 0;
  REG_GLB_B = 0;
  ws2812_setleds_constant((struct cRGB *)&REG_GLB_G, N_LEDS);
  REG_GLB_G = pgm_read_byte(init_color);
  REG_GLB_R = pgm_read_byte(init_color + 1);
  REG_GLB_B = pgm_read_byte(init_color + 2);
  
  // initialize all control registers
  REG_CTRL      = 0;
  REG_LED_CNT   = 1;
  REG_MAX_LED   = N_LEDS;
  REG_GLB_G     = 0;
  REG_GLB_R     = 0;
  REG_GLB_B     = 0;
  //REG_GLB_W   = 0;
  REG_GPIO_CTRL = 0;   
 
  // Reset the LED registers to zero 
  volatile uint8_t *p = i2c_reg + I2C_N_GLB_REG;
  uint16_t i = I2C_N_GLB_REG + N_LEDS*LED_COLS;
  while (i--) { *p = 0; p++; }
  // todo: replace with memset

  sei();
}

void setup(void) {

  DDRB = (1 << 3);

  i2c_init();
  initialize();
}

void loop(void) {

	if (i2c_check_stop()) {
	
	  if (REG_CTRL & CTRL_RST)         initialize();
	  else if (REG_CTRL & CTRL_GLB)    set_leds_global();
	  else if (REG_CTRL & CTRL_WAIT) {
		if (REG_CTRL & CTRL_SHOW ) {
		  update_leds();
		  REG_CTRL &=  ~CTRL_SHOW; // clear show bit
		} else { } // do nothning
	  } else							update_leds();
	  // todo: only update if LEDs are updated
	  
	} // if (i2c_check_stop()) {
}