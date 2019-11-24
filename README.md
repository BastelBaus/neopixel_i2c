# neopixel_i2c

This projects implements a way to control a WS2811/2 pixel array over 
I2C by using an slave module (e.g. digispark ATTiny85). The man class abstracts 
the comunication to the I2C slave to control the neopixels. Additionally there 
can be two general purpose IOs be controled in the slave over I2C.

The slave code can be found in the subdirectory [slave/](slave/).

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
| ATTiny85     | RGB    | ~140                   |
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


## Blink example

```
#include <neopixel_i2c.h>
#define LED_COUNT    2
#define I2C_ADDRESS  0x40

neopixel_i2c neoI2C( I2C_ADDRESS );

void setup() {
  neoI2C.setup(LED_COUNT); 
}

void loop() {

  for(uint8_t i = 0; i < neoI2C.getPixelCount() ; i++)
    neoI2C.setPixelColor( i, 0xFF,0xFF,0xFF);
  delay(300);
  
  neoI2C.clear();
  delay(300);
}
```

## Functional description

The following sections give a brief overview of teh functionality. You can check 
[Examples\FullTest\FullTest.ino](Examples\FullTest\FullTest.ino) to see a simple demo 
using all functionality for demonstration purpose.

### Initialization

The libraray must be configured with the I2C address of the slave device. Note the 
default I2C address is 0x40.

```
    #define I2C_ADR 0x40
	neopixel_i2c neoI2C(I2C_ADR);
```

YOu have to call the setup function within your setup routine to initialize the
I2C interface (i.e. Wire library). Optionally you can already configure the behavior 
if LED should be updated after each I2C command or only with the global show comman.

```
	#define LED_COUNT  100
	bool setup(LED_COUNT, WAIT_TO_SHOW);
    uint8_t ver = neoI2C.getVersion();     
```
	
The number of controlled LEDs can also be set dynamically. Note, if more 
LEDs are set as maximum supported by the slave, the setPixelCount() command
will not change anything.
 
```
	bool     setPixelCount(uint8_t pixelCount);
	uint8_t  getPixelCount(void);
    uint8_t  getMaxPixelCount();
```

### Setting LEDs

Pixels can be set individually or globally by either passing three bytes for red, 
green and blue or alternatively passing a cRGB structure including these colors. The 
global command is significantly faster than setting individual LEDs to the same color.
A clear command can be used to set all LED addresses to zero. By default this 
command will direcly update all LEDs.

```
    void setPixelColor(uint8_t ID, cRGB color); 
    void setPixelColor(uint8_t ID, uint8_t r, uint8_t g, uint8_t b); 
    void setPixelColorGlobal(cRGB color, bool show=true); 
    void setPixelColorGlobal(uint8_t r, uint8_t g, uint8_t b, bool show=true);
    void clear(bool show = true);     // set all pixels off, if show is = false, only in RAM
```

If the WAIT mode is set, the LEDs are only updated if the show() function is called. Using 
the wait mode can significantly speed up your code if you update several LEDs at one time.
```
	void setWaitMode(bool waitForShow=true);   
    void show(void);
```
	
### GPIO control

The GPIOs can be configured indicvidually using the configGPIO function (```neoI2C.configGPIO(GPIOa_OUTPUT | GPIOb_INPUT); ```).

```
    // Configure, set and get GPIOs
	#define GPIOa_OUTPUT		0x01
	#define GPIOa_INPUT			0x02
	#define GPIOa_INPUT_PULLUP  0x03
	#define GPIOb_OUTPUT		0x10
	#define GPIOb_INPUT			0x20
	#define GPIOb_INPUT_PULLUP  0x30	
	void    configGPIO(uint8_t config);
```

The GPIOs set or read by teh get and set function as follows (```neoI2C.setGPIO( neoI2C.getGPIO() );```).
	
```
	#define GPIOa_LOW  			0x01	
	#define GPIOa_HIGH 			0x02	
	#define GPIOb_LOW  			0x10	
	#define GPIOb_HIGH  		0x20	
	void    setGPIO(uint8_t level);
	uint8_t getGPIO(void);
```

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

Note: You can use the Wire.setClock function to change the I2C speed

*ToDo*: DO we have to wait for a new transfer or how is it handle in the master/slave ?


