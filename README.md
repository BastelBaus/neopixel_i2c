# neopixel_i2c

This projects implements a way to control a WS2811/2 pixel array over 
I2C by using an slave module (e.g. digispark ATTiny85). The man class abstracts 
the comunication to the I2C slave to control the neopixels. The slave code 
can be found in the subdirectory [slave/](slave/).

The project was started based on the slave code of [usedbytes/neopixel_i2c](https://github.com/usedbytes/neopixel_i2c).

## How many LEDs?

The maximum number depends on 
  * the amount of RAM available on your AVR 
  * the type of LED (RGB or RGBW) and
  * the supported address space (current implementation 339 LEDs with 1024 addresses)

| slave device | colors | max. controlable LEDs  |
|--------------|--------|------------------------|
| ATTiny45     | RGB    | ~82                    |
| ATTiny45     | RGBW   |  ?                     |
| ATTiny85     | RGB    | 167                    |
| ATTiny85     | RGBW   |  ?                     |
| *1)          | RGB    |                        |
| *1)          | RGBW   |                        |

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

For more details and deviating configurations see slave code in subfolder [slave/](slave/).

# The slave.

The slave device must be programmed with the code stored in the subfolder [slave/](slave/). 
By default it is configured to work with the default configuration of the master.

# The control class for the master.


## Initialization

I2C address
maximum pixel count
setting pixel count

## Setting LED colors

## Special functionalities

### Direct update versus combined update

### Direct update versus combined update

## Additional notes

* change of I2C speed

## **Timing**

* Changing a pixel over I2C with the default speed of 100kHz takes about 
500us (I2C address + Reg Address + 3 bytes) for the I2C transfer.

* Updating the LED strip takes about 35us per LED

Some example calculations are shown in the table below:

| total \# of LEDs | updated LEDs* | I2C speed  | I2C duration  | LED update time  | total time|
|------------------|---------------|------------|---------------|------------------|-----------|
|                1 |            1  |   100kHz   |       ~0.5ms  |           ~0.0ms |    ~0.5ms |
|              100 |            1  |   100kHz   |       ~0.5ms  |           ~0.4ms |    ~0.9ms |
|              100 |          100  |   100kHz   |      ~50.0ms  |           ~3.5ms |   ~53.5ms |
|                1 |            1  |   400kHz   |       ~0.1ms  |           ~0.0ms |    ~0.5ms |
|              100 |            1  |   400kHz   |       ~0.1ms  |           ~3.5ms |    ~0.9ms |
|              100 |          100  |   400kHz   |      ~12.5ms  |           ~3.5ms |   ~16.0ms |

*) assuming singe LED setting and and no special modes
 
*ToDo*: DO we have to wait for a new transfer or how is it handle in the master/slave ?


