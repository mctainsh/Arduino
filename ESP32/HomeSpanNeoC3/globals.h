#pragma once

// Maximum power is 255. 50 is a good value
//  88 = 2.9A on 308 LEDs
// 188 = 3.8A on 308 LEDssmart
#define MAX_POWER	(127)

//#define LED_COUNT 108		// Home Bed
#define LED_COUNT 315		// Long strip

// Which input switch pin on the Arduino is connected to the NeoPixels?
// Version 1 = 2
// Version 2 = 1
// Version 3 = 6
// LOLIN S2 Mini = 18
#define LED_PIN    2	

// Which input switchs the power onto the NeoPixels power?
// Version 3 = 5
#define STRIP_POWER 5	    		

// Red LED Pin to show status  9 Is blue, 10 is red
// Version 1 = 10
// Version 2 = 10
// Version 3 = 10
// LOLIN S2 Mini = 15
#define STATUS_LED_PIN 10

// Red LED Pin to show status  9 Is blue, 10 is red
// Version 1 = 9
// Version 2 = 9
// Version 3 = 9
// LOLIN S2 Mini = 0
#define CONTROL_SWITCH_PIN 9


#include "NeoPixel.h"

extern NeoPixel _neoPixel;
