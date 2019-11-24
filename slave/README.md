# neopixel_i2c_slave

This is an AVR-based neopixel driver slave. It accepts commands over I2C to drive a
a number of ws2811/2 LED pixels. It was based on the usedbytes/neopixel_i2c
implementation. Additional 2 IOs can be used as general purpose IOs over I2C.

For introduction see [BastelBaus/neopixel_i2c](https://github.com/BastelBaus/neopixel_i2c). 


## How many LEDs?


The maximum number depends on 
  * the amount of RAM available on your AVR 
  * the type of LED (RGB or RGBW) and
  * the supported address space 

| slave device | colors | max. controlable LEDs  |
|--------------|--------|------------------------|
| ATTiny45     | RGB    | ~82                    |
| ATTiny45     | RGBW   |  ?                     |
| ATTiny85     | RGB    | ~140+                  |
| ATTiny85     | RGBW   |  ?                     |
| *1)          | RGB    | 252                    |
| *1)          | RGBW   | 253                    |

*) Device with maximum RAM, only limited by address space.


## Circuit

Below is the circuit description for the default configured slave module in the subfolder slave/.

### Default pin assignment on a digisparc ATTiny85.

```
PB5: not used
PB4: GPIOb
PB3: NeoPixelData
PB2: I2C - SCL
PB1: GPIOa
PB0: I2C - SDA
```

For more details and deviating configurations see slave code in subfolder [slave/](slave/).
For product description, see [http://digistump.com/products/1](http://digistump.com/products/1).

![digisparc with ATTiny85 as example slave device](https://github.com/BastelBaus/neopixel_i2c/blob/master/digispark_ATTiny85.png)

### Suggested circuit for own implementation.

```
                            ^ VCC
                            |
    +--------+--------------+-------------+-----+-------+
    |        |                            |     |       |
    |       |"| 10k                       |     |       |
    |       |_|                           |     |       |
    |        |     +Attinyx5--------+     |    |"| 2k2 |"| 2k2
    |        +-----| Reset      Vcc |-----'    |_|     |_|
    |              |                |           |       |
  _____ 0.1u       |                |           |       |
  _____  ,---------|            SCL |-----------+-------|-----<> SCL
    |    |         |                |                   |
    |    |         |                |                   |
    |    | GPIOb<--|                |-->GPIOa           |
    |    |         |                |                   |
    |    |         |                |                   |
    +----|---+-----| GND        SDA |-------------------+-----<> SDA
         |   |     +----------------+
         |   v GND
         |
         '-----------------------------------------------------> NeoPixel Data

```

## Functionality

Over I2C several control register can be used to setup and control the behavior of the slave. The slave has 
a maximum number address spaces for the LEDs (depening on RAM). The used LEDs can be configured over a register.
Each pixel can be addressed seprately and updated directly or with a global LED update commad. The GPIOs can be 
configured to output or inputs and a bit can be used to read the input value and to set the output value 
respectively.


### **on reset**

All registers are set to their initial values and the LEDs (maximum numbers configures in the slave) are set to off.

## i2c Protocol

**The slave address is currently hardcoded to 0x40 in the firmware**, see ```i2c_slave_defs.h:28 ```.

This utilises the i2c slave library (https://github.com/usedbytes/usi_i2c_slave)
which means it follows a fairly standard i2c protocol (for more, see here:
http://www.robot-electronics.co.uk/i2c-tutorial).

Writes look like this:

| Start | Slave Address << 1 | register address | Data | Stop |
|-------|--------------------|------------------|------|------|

First you sent the 8bit register address followed by the n times 8bit data. The address is 
incremented by one each time a data is written to allow bust writes.

Reads look like this:

| Start | Slave Address | register address | Restart | (Slave Address << 1) + 1  | Data | Stop |
|-------|---------------|------------------|---------|---------------------------|------|------|

First you do a write transaction to set the 8bit register address to read from, then
a read transaction to read the data. After each read byte the address is incremented by one.
When you've read all the data you want, send a NAK after the last byte to terminate the read.

Notes:
* The registzer address is multiplied at the slave with the 
	multiplication factor k to determine the starting real register 
	address. This allows to address more than 256 registers (maximum
	4*256=1024)
* In non WAIT mode, the LED values are only updated after a STOP is received
* Each time the LEDs are updated, the I2C is not responsive. See last section 
    for detailed timing description.

## Register Map

The register map consists of a number of global control registers - address
0x00-0x0C - followed by an array of registers which hold the individual value
for each LED in normal mode (3 for RGB and 4 for RGBW).

### Address table for RGB Mode 

| **Address** | **Name**       | **Description**      | **Access** | **Reset** |
|------------:|----------------|:---------------------|--------|------:|
|   0x00      | **CTRL0**      | Control Register 0   | R/W    |    0  |
|   0x01	  | **CTRL1**      | Control Register 1   | R/W    |    0  |
|   0x02      | **LED_CNT**    | used number of LEDs  | R/W    |    1  |
|   0x03      | **MAX_LED**    | max number of LEDs   | R      |  MAX  |
|   0x04      | **GLB_G**      | Global Green Value   | R/W    | 0x00  |
|   0x05      | **GLB_R**      | Global Red Value     | R/W    | 0x00  |
|   0x06      | **GLB_B**      | Global Blue Value    | R/W    | 0x00  |
|   0x07      | **GLB_W**      | Global Blue Value    | R/W    |    0  |
|   0x08      | **VER**        | Slave SW version     | R/W    | 0x01  |
|   0x09      | RSV            | Reserved             | R/W    |    0  |
|   0x0A      | RSV            | Reserved             | R/W    |    0  |
|   0x0B      | RSV            | Reserved             | R/W    |    0  |
| RGB Array   |----------------|----------------------|--------|-------|
|   0x0C      | **GREEN[0]**   | Green value, LED0    | R/W    |    0  |
|   0x0D      | **RED[0]**     | Red value, LED0      | R/W    |    0  |
|   0x0E      | **BLUE[0]**    | Blue value, LED0     | R/W    |    0  |
|   0x0F      | **GREEN[1]**   | Green value, LED1    | R/W    |    0  |
|   ....      |     ....       | ....                 | ....   | ....  |
|  3*n + 0x0C | **GREEN[n]**   | Green value, LEDn    | R/W    |    0  |
|  3*n+1+0x0C | **RED[n]**     | Red value, LEDn      | R/W    |    0  |
|  3*n+2+0x0C | **BLUE[n]**    | Blue value, LEDn     | R/W    |    0  |
|   ....      | ....           | ....                 | ....   | ....  |

### Address table for RGBW Mode 

| **Address** | **Name**       | **Description**      | **Access** | **Reset** |
|------------:|----------------|:---------------------|--------|------:|
|       	  |   |         |       |      |
|   as above  | as above       | as above     |     |      |
|       	  |   |         |       |      |
| RGBW Array  |----------------|----------------------|--------|-------|
|   0x0C      | **GREEN[0]**   | Green value, LED0    | R/W    |    0  |
|   0x0D      | **RED[0]**     | Red value, LED0      | R/W    |    0  |
|   0x0E      | **BLUE[0]**    | Blue value, LED0     | R/W    |    0  |
|   0x0F      | **WHITE[0]**   | Blue value, LED0     | R/W    |    0  |
|   0x10      | **GREEN[1]**   | Green value, LED1    | R/W    |    0  |
|   ....      | ....           | ....                 | ....   | ....  |
| 4*n + 0x0C  | **GREEN[n]**   | Green value, LEDn    | R/W    |    0  |
| 4*n+1+0x0C  | **RED[n]**     | Red value, LEDn      | R/W    |    0  |
| 4*n+2+0x0C  | **BLUE[n]**    | Blue value, LEDn     | R/W    |    0  |
| 4*n+3+0x0C  | **WHITE[n]**   | White value, LEDn    | R/W    |    0  |
|   ....      | ....           | ....                 | ....   | ....  |

## Register Descriptions

### Control registers **CTRL0** and **CTRL1** 

The both control register sets the operating mode and special operations.

|  CTRL0: |   M1 |   M0 |  GRB | SETG | SHOW | WAIT | RGBW | RST  |
|--------:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|    Bit: |   7  |    6 |    5 |    4 |    3 |    2 |    1 |    0 |
| Access: |   rw |   rw |   rw |   rw |   rw |   rw |  rw  |  rw  |
| Reset:  |    0 |   0 |     0 |    0 |    0 |    0 |   0  |   0  |

|  CTRL1: |400khz|  RSV | IOb2 | IOb1 | IOb0 | IOa2 | IOa1 | IOa0 |
|--------:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|    Bit: |   7  |    6 |    5 |    4 |    3 |    2 |    1 |    0 |
| Access: |   rw |   rw |   rw |   rw |   rw |   rw |  rw  |  rw  |
| Reset:  |    0 |    0 |    0 |    0 |    0 |    0 |   0  |   0  |

A short description of the control register bits is depiced in the following table. A detailed description follows in the sections below.

| Bits  | # | description                                                    |
|-------|---|----------------------------------------------------------------| 
| RST   | 1 | 1: software reset the slave
| RGBW  | 1 | 0: RGB mode, 1: RGBW mode (not supported in version 0x01
| WAIT  | 1 | 0: direcly update pixels after each command
|       |   | 1: only update on when show bit is set
| SHOW  | 1 | 1: update all pixels
| SETG  | 1 | 1: set all pixels to GLB color
| GRB   | 1 | 0: GRB mode (default) // 1: RGB neopixel color mode (not yet implemented)
| M1:M0 | 2 | scale ADR by factor k ( 0: k=1, 1: k=2, 2:k=3, 3: k=4)
| IOa1:0| 2 | GPIO_a [ b00: INPUT, b01: INPUT_PULLUP, B10: OUTPUT  B11: RESERVED ]
| IOa2  | 2 | GPIO_a read/write value
| IOb1:0| 2 | GPIO_b [ b00: INPUT, b01: INPUT_PULLUP, B10: OUTPUT  B11: RESERVED ]
| IOb2  | 2 | GPIO_b read/write value
| 400khz| 1 | 0: 800kHz mode (default)  // 1: 400kHZ mode (not yet implemented)

#### *RST*

Writing a 1 to this bit will reset the LED controler, setting all LEDs to OFF and 
all register to their default values.  This bit will be automatically cleared once 
the reset has completed.

#### **RGBW** 

Configure the connected neopixel to RGB and RGBW mode respectively.

0: Default RGB mode(default)
1: RGBW mode (not yet implemented

#### **WAIT** 

Change the mode. In WAIT mode, the LEDs are only updated if an explicit show 
command used. In default mode (not WAIT), pixels are updated after each I2C 
command automatically.

0: WAIT mode turned off. All LEDs are updated after the STOP of each(!) I2C command.
1: WAIT mode turned on. All LEDs are only updated if a SHOW command is sent.

#### **SHOW** 

Updates the LEDs with the current colors in local RAM. This bit is reset to 0 
after sending the command to the LEDS


#### *SETG*

Writing a one to this bit fill the complete RAM with the global color (GLB_R, GLB_G, GLB_B). 
After the RAM is filled, the bit is reset to 0.

#### Address multiplication **M1**, **M0** 

M1 and M0 are the multiplication factors to address more than 256 addresses (e.e. more than 84 LEDs in RGB mode).

| M1 | M0 | multiplication factor k | 
|----|----|-------------------------|
|  0 |  0 | k=1 (default)           |
|  0 |  1 | k=2                     |
|  1 |  0 | k=3                     |
|  1 |  1 | k=4                     |


#### GPIO control **IOa2:0** and **IOa2:0**

With this register bits, the remaining two IO ports can be configured. The IOa/b2 
register is a read only and write register bit depending on the PIN configuration.


| IOb2 | IOb1 | IOb0 | IOa2 | IOa1 | IOa0 | description                       | 
|------|------|------|------|------|------|-----------------------------------| 
|    * |    * |    * |    * |    0 |    0 | GPIO_a is a INPUT pin             |
|    * |    * |    * |    * |    0 |    1 | GPIO_a is a INPUT pin with PULLUP |
|    * |    * |    * |    * |    1 |    0 | GPIO_a is a OUTPUT                |
|    * |    * |    * |    * |    1 |    1 | RESERVED                          |
|    * |    * |    * |    0 |    * |    * | INPUT(read only) low level        |
|      |      |      |      |      |      | OUTPUT(write only) set low level  |
|    * |    * |    * |    1 |    * |    * | INPUT(read only) high level       |
|      |      |      |      |      |      | OUTPUT(write only) set high level |
|------|------|------|------|------|------|-----------------------------------| 
|    * |    0 |    0 |    * |    * |    * | GPIO_b is a INPUT pin             |
|    * |    0 |    1 |    * |    * |    * | GPIO_b is a INPUT pin with PULLUP |
|    * |    1 |    0 |    * |    * |    * | GPIO_b is a OUTPUT                |
|    * |    1 |    1 |    * |    * |    * | RESERVED                          |
|    0 |    * |    * |    * |    * |    * | INPUT(read only) low level        |
|      |      |      |      |      |      | OUTPUT(write only) set low level  |
|    1 |    * |    * |    * |    * |    * | INPUT(read only) high level       |
|      |      |      |      |      |      | OUTPUT(write only) set high level |

#### future extentions (**GRB** and **400KHZ**)

Note: not yet implemented
* NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
* NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
* NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
* NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
* NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


### **GLB_R**, **GLB_G**, **GLB_B**, **GLB_W**

These registers hold the global colour value. It can be set using teh **SETG** command 
bit in the **CTRL0** register.

### **VER** Version

Version is set to 0x01 in the current stream. Will be counted up for future major relases.

### **LED Value Array**
Everything after the global registers is an array of data for each LED.


