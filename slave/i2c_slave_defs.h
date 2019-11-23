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
#define I2C_SLAVE_ADDR 	  0x40
#define N_LEDS 			      140 			
#define TOTAL_RAM         512
#define LED_COLS			      3



// check if sufficient free RAM is available
#define FEE_STACK_NEED   80  // not calculated, just guessed
#if (N_LEDS * LED_COLS + FEE_STACK_NEED)> TOTAL_RAM 
  #error "LED count To high. It exceeds maximum RAM."
#endif



/*
 * The library supports a write mask for each individual register (bits set are
 * writable) in the i2c_w_mask array. If you don't care about masks for each
 * individual register, you can define a global value to be used for all
 * registers here, saving flash and RAM
 */
#define I2C_GLOBAL_WRITE_MASK 0xFF

// set, test and clear shortcuts for master
#define ISBITSET(BYTE,BIT) 	 ((BYTE & (1<<BIT)) != 0)
#define SETBIT(BYTE,BIT)   	 BYTE = BYTE | (1<<BIT)
#define CLRBIT(BYTE,BIT)   	 BYTE = BYTE & (~(1<<BIT))

// set, test and clear shortcuts for slave using i2c_reg variable
#define ISBITSETREG(REG,BIT) ISBITSET(i2c_reg[REG],BIT)
#define SETBITREG(REG,BIT)   SETBIT(i2c_reg[REG],BIT)
#define CLRBITREG(REG,BIT)   CLRBIT(i2c_reg[REG],BIT)
#define SETREG(REG,BYTE)     i2c_reg[REG]=BYTE

// control register addresses and bits 
#define REG_CTRL0      0
  #define CTRL0_RST      0 // 
  #define CTRL0_RGBW     1
  #define CTRL0_WAIT     2 // 0: update each pixel after each command. 1: only updates pixels if CTRL_SHOW set is set
  #define CTRL0_SHOW     3 // 1: all LEDs are set, after finish, value is reset to 0
  #define CTRL0_SETG     4 // M1:M0 = B00 ==> k=1   // M1:M0 = B01 ==> k=2
  #define CTRL0_M0       6 // M1:M0 = B00 ==> k=1   // M1:M0 = B01 ==> k=2
  #define CTRL0_M1       7 // M1:M0 = B10 ==> k=3   // M1:M0 = B00 ==> k=4
  #define GET_CTRL_k     ( ( (i2c_reg[REG_CTRL0] &  ((1<<CTRL0_M1)|(1<<CTRL0_M0))) >>6 )  + 1 )
  
#define REG_CTRL1      1
  #define CTRL1_IOa0     0
  #define CTRL1_IOa1     1
  #define CTRL1_IOa2     2
  #define CTRL1_IOb0     3
  #define CTRL1_IOb1     4
  #define CTRL1_IOb2     5
  
#define REG_LED_CNT    2
#define REG_MAX_LED    3
#define REG_GLB_G      4
#define REG_GLB_R      5
#define REG_GLB_B      6
#define REG_GLB_W      7
#define REG_VER        8

#define REG_FIRSTLED  12 // should be dividable by 2, 3 and 4	

#define I2C_N_REG 	   (REG_FIRSTLED + (N_LEDS * LED_COLS))
#define I2C_N_GLB_REG   REG_FIRSTLED

#endif /* __I2C_SLAVE_DEFS__ */