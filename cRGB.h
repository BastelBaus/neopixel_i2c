#ifndef CRGB_H
#define CRGB_H

/*
Control a RGB led with Hue, Saturation and Brightness (HSB / HSV )

Hue is change by an analog input.
Brightness is changed by a fading function.
Saturation stays constant at 255

getRGB() function based on <http://www.codeproject.com/miscctrl/CPicker.asp>
dim_curve idea by Jims

created 05-01-2010 by kasperkamperman.com
*/

/*
dim_curve 'lookup table' to compensate for the nonlinearity of human vision.
Used in the getRGB function on saturation and brightness to make 'dimming' look more natural.
Exponential function used to create values below :
x from 0 - 255 : y = round(pow( 2.0, x+64/40.0) - 1)
*/

// uncomment this line if you use HSV is many projects
#define USE_HSV	1

struct cRGB { 
	uint8_t g; 
	uint8_t r; 
	uint8_t b;

};

#ifdef USE_HSV

const uint8_t dim_curve[] = {
//    const uint8_t PROGMEM gamma8[] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
        2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
        5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
       10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
       17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
       25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
       37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
       51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
       69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
       90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
      115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
      144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
      177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
      215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
	/*  
const uint8_t dim_curve[] = {
	0, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6,
	6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8,
	8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11,
	11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15,
	15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20,
	20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26,
	27, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35,
	36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 47,
	48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
	63, 64, 65, 66, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 81, 82,
	83, 85, 86, 88, 90, 91, 93, 94, 96, 98, 99, 101, 103, 105, 107, 109,
	110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
	146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
	193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};
*/
#define MAX_HUE 360
#define DIV_HUE (MAX_HUE / 6) 

// HUE: [0..MAX_HUE)
static cRGB getRGBfromHSV(uint16_t hue, uint8_t sat=0xff, uint8_t val=0xff) {
  cRGB  col;

  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
 	 The dim_curve is used only on brightness/value and on saturation (inverted).
	 This looks the most natural.
	 */

	val = dim_curve[val];
	sat = 255 - dim_curve[255 - sat];

	int base;

	if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
		col.r = val;
		col.g = val;
		col.b = val;
	}
	else  {
		base = ((( uint16_t)255 - sat) * val) >> 8;

		switch (hue / DIV_HUE ) {
		case 0:
			col.r = val;
			col.g = (((val - base)*hue) / DIV_HUE) + base;
			col.b = base;
			break;

		case 1:
			col.r = (((val - base)*(DIV_HUE - (hue % DIV_HUE))) / DIV_HUE) + base;
			col.g = val;
			col.b = base;
			break;

		case 2:
			col.r = base;
			col.g = val;
			col.b = (((val - base)*(hue % DIV_HUE)) / DIV_HUE) + base;
			break;

		case 3:
			col.r = base;
			col.g = (((val - base)*(DIV_HUE - (hue % DIV_HUE))) / DIV_HUE) + base;
			col.b = val;
			break;

		case 4:
			col.r = (((val - base)*(hue % DIV_HUE)) / DIV_HUE) + base;
			col.g = base;
			col.b = val;
			break;

		case 5:
			col.r = val;
			col.g = base;
			col.b = (((val - base)*(DIV_HUE - (hue % DIV_HUE))) / DIV_HUE) + base;
			break;
		}			
	}
	return col;
}

	
#endif


#endif
