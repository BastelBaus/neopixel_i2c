# neopixel_i2c_slave (picopixel)

This is an AVR-based neopixel driver. It accepts commands over I2C to drive a
a number of ws2811/2 LED pixels. It was based on the usedbytes/neopixel_i2c
implementation. It consists of a tiny slave module (Arduino IDE) and a control 
class which can be included on the master.


## How many LEDs?

The maximum number depends on 
  * the amount of RAM available on your AVR (Attiny45 ~82 LEDs, and an Attiny85 ~167 each for RGB)
  * the type of LED (RGB or RGBW) and
  * the supported address space (current implementation 338 LEDs with 1024 addresses)

## Circuit


### Defaultr Pin assignment on a digisparc ATTiny85.

PB6: not used
PB5: GPIO2
PB4: GPIO1
PB3: NeoPixelData
PB2: I2C - SCL
PB1: GPIO0
PB0: I2C - SDA

### Suggested circuit from the scratch.

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
    |    |       --|                |--                 |
    |    |         |                |                   |
    |    |         |                |                   |
    +----|---+-----| GND        SDA |-------------------+-----<> SDA
         |   |     +----------------+
         |   v GND
         |
         '-----------------------------------------------------> NeoPixel Data

```

## Getting the code

This project uses git submodules (I kinda wish it didn't...). You can clone
it like so:

```git clone --recursive https://github.com/usedbytes/neopixel_i2c```

If your git version is too old to support that, do this instead:

```
git clone --recursive https://github.com/usedbytes/neopixel_i2c
cd neopixel_i2c
git submodule update --init --recursive
```


## Basic Functionality

There are two basic operating modes:
 * In *normal* mode, each LED is individually driven based on the value in its
   control register. All LEDs are updated either
     ** when an I2C command finishes
	 ** or in WAIT mode is set when a update command is issued
 * In *global* mode, all LEDs are driven to the same value, based on the values
   in the global value registers.

The operating modes are selected by flipping the *GLB* bit in the **CTRL**
register.

## i2c Protocol

**The slave address is currently hardcoded to 0x40 in the firmware**, see

```
i2c/i2c_slave_defs.h:30
```


This utilises my i2c slave library (https://github.com/usedbytes/usi_i2c_slave)
which means it follows a fairly standard i2c protocol (for more, see here:
http://www.robot-electronics.co.uk/i2c-tutorial).

Writes look like this:

| Start | Slave Address << 1 | Register Address | Data | Stop |
|-------|--------------------|------------------|------|------|

There is a 10bit address space from which the lower 8 bits are included in each 
command and the upper two bits have to be set in the global control register.
The register address will auto-increment after every byte, so you can write / read
data in bursts.

Reads look like this:

| Start | Slave Address | Register Address | Restart | (Slave Address << 1) + 1  | Data | Stop |
|-------|---------------|------------------|---------|---------------------------|------|------|

First you do a write transaction to set the register address to read from, then
a read transaction to read the data. When you've read all the data you want,
send a NAK after the last byte to terminate the read.

**In non WAIT mode, The LED values are only updated after a STOP is received**

**Each time the LEDs are updated, the I2C is not responsice. It takes about xxx ms / 
LED (xms/10LEDs) until new I2C commands can be received.**

## Register Map
The register map consists of a number of global control registers - address
0x000-0x004 - followed by an array of registers which hold the individual value
for each LED in normal mode.

### Address table for RGB Mode 

| **Address** | **Name**     | **Description**      | **Access** | **Reset** |
|------------:|--------------|:---------------------|--------|------:|
|   0x000      | **CTRL**     | Control Register     | R/W    |    0  |
|   0x001      | **GLB_G**    | Global Green Value   | R/W    |    0  |
|   0x002      | **GLB_R**    | Global Red Value     | R/W    |    0  |
|   0x003      | **GLB_B**    | Global Blue Value    | R/W    |    0  |
|   0x004      | **LED_CNT**  | total number of LEDs | R/W    |    0  |
|Array follows|--------------|----------------------|--------|-------|
|   0x005      | **GREEN[0]** | Green value, LED0    | R/W    |    0  |
|   0x006      | **RED[0]**   | Red value, LED0      | R/W    |    0  |
|   0x007      | **BLUE[0]**  | Blue value, LED0     | R/W    |    0  |
|   ....      | ....         | ....                 | ....   | ....  |
| (3*n) + 4   | **GREEN[n]** | Green value, LEDn    | R/W    |    0  |
| (3*n) + 5   | **RED[n]**   | Red value, LEDn      | R/W    |    0  |
| (3*n) + 6   | **BLUE[n]**  | Blue value, LEDn     | R/W    |    0  |
|   ....      | ....         | ....                 | ....   | ....  |
| 0x3FB   | **GREEN[338]** | Green value, LEDn    | R/W    |    0  |
| 0x3FC   | **RED[338]**   | Red value, LEDn      | R/W    |    0  |
| 0x3FD   | **BLUE[338]**  | Blue value, LEDn     | R/W    |    0  |
| 0x3FE   | **RSVD** | reserved | R/W    |    0  |
| 0x3FF   | **RSVD**  | reserved     | R/W    |    0  |

### Address table for RGBW Mode 

t.b.d.

## Register Descriptions

### **CTRL**
The control register sets the operating mode.

|   Name: | A9   |   A8 | RSVD | RGBW | WAIT | SHOW | GLB  | RST  |
|--------:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|    Bit: |   7  |    6 |    5 |    4 |    3 |    2 |    1 |    0 |
| Access: |   rw |   rw |    r |   rw |   rw |   rw |  rw  |  rw  |
| Reset:  |    0 |   0 |     0 |    0 |    0 |    0 |   0  |   0  |

#### *RST*
Writing a 1 to this bit will reset the LED controler, setting all LEDs to OFF

This bit will be automatically cleared once the reset has completed.

#### *GLB*
Writing a one to this bit causes the global color value to be displayed on all
LEDs at the end of the transaction - Normally you would set the **GLB_R**,
**GLB_G**, and **GLB_B** values in the same transaction as setting the *GLB*
bit so that the new colour is immediately applied.

Writing a zero to this bit will disable the global colour override and return to
normal operation.


#### **SHOW** 
Updates the LEDs with the current colors in local RAM. This bit is reset to 0 
after sending the command to the LEDS

#### **RGBW** 

0: Default RGB mode(default)
1: RGBW mode

#### WAIT 

0: WAIT mode turned off. All LEDs are updated after the STOP of the I2C command
1: WAIT mode turned on. All LEDs are only updated if a SHOW command is sent.


#### **A9**, **A8** 
High bits of the register map.

Note: with a Burst command you can also write the complete address 
range w/o change the upper address bits explicitly


### **GLB_R**, **GLB_G**, **GLB_B**
These registers hold the global colour value. When the *GLB* bit in the
**CTRL** register is set, all LEDs will display this colour.


### **LED Value Array**
Everything after the global registers is an array of data for each LED.
When the *GLB* bit is not set, each LED will display whatever value is
programmed in its corresponding register set.
