/*
 * Copyright Brian Starkey 2014 <stark3y@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __I2C_SLAVE_DEFS__
#define __I2C_SLAVE_DEFS__

/* Set these appropriately for your platform */
#define USI_PORT 	PORTB
#define USI_DDR 	DDRB
#define I2C_SDA 	0
#define I2C_SCL 	2

// I2C address and maximum number of LEDs
#define I2C_SLAVE_ADDR 		0x40
#define N_LEDS 				80 			// ((uint8_t)((2^8-4))/3))
#define LED_COLS			3			// 3: for RGB 4: RGBW

//#define N_LEDS (15+21+15+21+6+6)  
//#define MAX_N_LEDS (2^16-4)  // limitation by 16 bit address

/*
 * The library supports a write mask for each individual register (bits set are
 * writable) in the i2c_w_mask array. If you don't care about masks for each
 * individual register, you can define a global value to be used for all
 * registers here, saving flash and RAM
 */
#define I2C_GLOBAL_WRITE_MASK 0xFF

#define I2C_N_GLB_REG 	12		// should be dividable by 2, 3 and 4	
#define I2C_N_REG 		(I2C_N_GLB_REG + (N_LEDS * LED_COLS))

#define REG_CTRL    i2c_reg[0]
  #define CTRL_RST    (1 << 0) // 
  #define CTRL_GLB    (1 << 1)
  #define CTRL_WAIT   (1 << 2) // 0: update each pixel after each command. 1: only updates pixels if CTRL_SHOW set is set
  #define CTRL_SHOW   (1 << 3) // 1: all LEDs are set, after finish, value is reset to 0
  #define CTRL_M0     (1 << 6) // M1:M0 = B00 ==> k=1   // M1:M0 = B01 ==> k=2
  #define CTRL_M1     (1 << 7) // M1:M0 = B10 ==> k=3   // M1:M0 = B00 ==> k=4
  #define GET_CTRL_k  ( ( (REG_CTRL &  (CTRL_M1|CTRL_M0)) >>6 )  + 1 )
  
#define REG_LED_CNT    i2c_reg[1]
#define REG_MAX_LED    i2c_reg[2]
#define REG_GLB_G      i2c_reg[3]
#define REG_GLB_R      i2c_reg[4]
#define REG_GLB_B      i2c_reg[5]
#define REG_GPIO_CTRL  i2c_reg[6]
   // Configure, set and get GPIOs
  #define REG_GPIO0_CTRL    0
  #define REG_GPIO0_IO      2
  #define REG_GPIO1_CTRL    3
  #define REG_GPIO1_IO      5 
  #define GPIO_OUTPUT       0
  #define GPIO_INPUT        1
  #define GPIO_INPUT_PULLUP 2
  
#define REG_FIRSTLED   i2c_reg[I2C_N_GLB_REG]

#endif /* __I2C_SLAVE_DEFS__ */