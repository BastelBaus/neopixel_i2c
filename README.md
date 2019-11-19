# neopixel_i2c

This class is to abstract the comunication to the I2C slave to 
control the neopixels. The slave code can be found in the subdirectory slave/.

It was based on the slave code of usedbytes/neopixel_i2c.

## How many LEDs?

The maximum number depends on 
  * the amount of RAM available on your AVR (Attiny45 ~82 LEDs, and an Attiny85 ~167 each for RGB)
  * the type of LED (RGB or RGBW) and
  * the supported address space (current implementation 339 LEDs with 1024 addresses)

## Circuit

Below is the circuit description for the default configured slave module in the subfolder slave/.

### Default pin assignment on a digisparc ATTiny85.

```
PB5: not used
PB4: GPIO1
PB3: NeoPixelData
PB2: I2C - SCL
PB1: GPIO0
PB0: I2C - SDA
```

For more details and deviating configurations see slave code in subfolder [slave/](slave/).
For product dewscription, see [http://digistump.com/products/1](http://digistump.com/products/1).


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
    |    | GPIO1<--|                |-->GPIO0           |
    |    |         |                |                   |
    |    |         |                |                   |
    +----|---+-----| GND        SDA |-------------------+-----<> SDA
         |   |     +----------------+
         |   v GND
         |
         '-----------------------------------------------------> NeoPixel Data

```

For more details and deviating configurations see slave code in subfolder slave/.

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

### **on reset**

All registers are set to their initial values and the LEDs (maximum numbers configures in the slave) are set to off.

## i2c Protocol

**The slave address is currently hardcoded to 0x40 in the firmware**, see

```
i2c/i2c_slave_defs.h:30
```


This utilises my i2c slave library (https://github.com/usedbytes/usi_i2c_slave)
which means it follows a fairly standard i2c protocol (for more, see here:
http://www.robot-electronics.co.uk/i2c-tutorial).

Writes look like this:

| Start | Slave Address << 1 | RegAdrPointer    | Data | Stop |
|-------|--------------------|------------------|------|------|

The RegAdrPointer is multiplied at the slave with the 
multiplication factor k to determine the starting register address.


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
0x00-0x004 - followed by an array of registers which hold the individual value
for each LED in normal mode.

### Address table for RGB Mode 

| **Address** | **Name**       | **Description**      | **Access** | **Reset** |
|------------:|----------------|:---------------------|--------|------:|
|   0x00      | **CTRL**       | Control Register     | R/W    |    0  |
|   0x01	  | **GPIO_CTRL**  | Control GPIOs        | R      |    0  |
|   0x02      | **LED_CNT**    | total number of LEDs | R/W    |    0  |
|   0x03      | **MAX_LED**    | total number of LEDs | R      |    0  |
|   0x04      | **GLB_G**      | Global Green Value   | R/W    |    0xFF  |
|   0x05      | **GLB_R**      | Global Red Value     | R/W    |    0xFF  |
|   0x06      | **GLB_B**      | Global Blue Value    | R/W    |    0xFF  |
|Array follows|----------------|----------------------|--------|-------|
|   0x07      | **GREEN[0]**   | Green value, LED0    | R/W    |    0  |
|             | **RED[0]**     | Red value, LED0      | R/W    |    0  |
|             | **BLUE[0]**    | Blue value, LED0     | R/W    |    0  |
|   0x04      | **GREEN[1]**   | Green value, LED1    | R/W    |    0  |
|   ....      | ....           | ....                 | ....   | ....  |
|   n + 3     | **GREEN[n]**   | Green value, LEDn    | R/W    |    0  |
|             | **RED[n]**     | Red value, LEDn      | R/W    |    0  |
|             | **BLUE[n]**    | Blue value, LEDn     | R/W    |    0  |
|   ....      | ....           | ....                 | ....   | ....  |
|   0xFF      | **GREEN[252]** | Green value, LED252  | R/W    |    0  |
|             | **RED[252]**   | Red value, LED252    | R/W    |    0  |
|             | **BLUE[252]**  | Blue value, LED252   | R/W    |    0  |
|-------------|----------------|----------------------|--------|-------|

### Address table for RGBW Mode 

| **Address** | **Name**       | **Description**      | **Access** | **Reset** |
|------------:|----------------|:---------------------|--------|------:|
|   0x00      | **CTRL**       | Control Register     | R/W    |    0  |
|       	  | **GPIO_CTRL**  | Control GPIOs        | R      |    0  |
|   	      | **LED_CNT**    | total number of LEDs | R/W    |    0  |
|             | **MAX_LED**    | total number of LEDs | R      |    0  |
|   0x01      | **GLB_G**      | Global Green Value   | R/W    |    0  |
|             | **GLB_R**      | Global Red Value     | R/W    |    0  |
|             | **GLB_B**      | Global Blue Value    | R/W    |    0  |
|             | **GLB_W**      | Global White Value   | R/W    |    0  |
|Array follows|----------------|----------------------|--------|-------|
|   0x03      | **GREEN[0]**   | Green value, LED0    | R/W    |    0  |
|             | **RED[0]**     | Red value, LED0      | R/W    |    0  |
|             | **BLUE[0]**    | Blue value, LED0     | R/W    |    0  |
|             | **WHITE[0]**   | Blue value, LED0     | R/W    |    0  |
|   0x04      | **GREEN[1]**   | Green value, LED1    | R/W    |    0  |
|   ....      | ....           | ....                 | ....   | ....  |
|   n + 3     | **GREEN[n]**   | Green value, LEDn    | R/W    |    0  |
|             | **RED[n]**     | Red value, LEDn      | R/W    |    0  |
|             | **BLUE[n]**    | Blue value, LEDn     | R/W    |    0  |
|             | **WHITE[n]**   | White value, LEDn    | R/W    |    0  |
|   ....      | ....           | ....                 | ....   | ....  |
|   0xFF      | **GREEN[253]** | Green value, LED253  | R/W    |    0  |
|             | **RED[253]**   | Red value, LED253    | R/W    |    0  |
|             | **BLUE[253]**  | Blue value, LED253   | R/W    |    0  |
|             | **WHITE[253]** | White value, LED253  | R/W    |    0  |

## Register Descriptions

### **CTRL**
The control register sets the operating mode.

|   Name: |   M1 |   M0 |  CLR | RGBW | WAIT | SHOW | GLB  | RST  |
|--------:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|    Bit: |   7  |    6 |    5 |    4 |    3 |    2 |    1 |    0 |
| Access: |   rw |   rw |   rw |   rw |   rw |   rw |  rw  |  rw  |
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

#### **WAIT** 

0: WAIT mode turned off. All LEDs are updated after the STOP of the I2C command
1: WAIT mode turned on. All LEDs are only updated if a SHOW command is sent.

#### **CLR** 

0: 
1: 

#### **RGBW** 

0: Default RGB mode(default)
1: RGBW mode

#### **M1**, **M0** 
>M1 and M0 are the multiplication factors to address more than 84 LEDs in RGB mode 
and x LEDs in RGBW mode respectively

| M1 | M0 | multiplication factor k | 
|----|----|-------------------------|
|  0 |  0 | k=1 (default)           |
|  0 |  1 | k=3 (default)           |
|  1 |  0 | k=4 (default)           |
|  1 |  1 | RESERVED                |

With this logic, 252 LEDs could be addressed maximum.

Note: The address is always incremented after a write/read with 1


### **GLB_R**, **GLB_G**, **GLB_B**
These registers hold the global colour value. When the *GLB* bit in the
**CTRL** register is set, all LEDs will display this colour.

### **GPIO_CTRL**
WIth this register, teh remaoning two IO ports can be configured

### **LED Value Array**
Everything after the global registers is an array of data for each LED.
When the *GLB* bit is not set, each LED will display whatever value is
programmed in its corresponding register set.

## **Timing*

Chaning a pixel over I2C takes 500us (I2C address + REg Address + 3 bytes)
 + optinoally: 2* (
 100 LEDs = 50ms
 
 
 setting 5 LEDs takes 190us
  80 LEDs takes 2.8ms
  35us per LED
 

