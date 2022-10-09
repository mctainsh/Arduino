#pragma once

// Maximum power is 255. 50 is a good value
#define MAX_POWER	(127)

//#define LED_COUNT 108		// Home Bed
#define LED_COUNT 300		// Long strip

// Which input switch pin on the Arduino is connected to the NeoPixels?
// Version 1 = 2
// Version 2 = 1
// Version 3 = 6
#define LED_PIN    6	

// Which input switchs the power onto the NeoPixels power?
// Version 3 = 5
#define STRIP_POWER 5	    		

// Red LED Pin to show status  9 Is blue, 10 is red
// Version 1 = 10
// Version 2 = 10
// Version 3 = 10
#define STATUS_LED_PIN 10

// Red LED Pin to show status  9 Is blue, 10 is red
// Version 1 = 9
// Version 2 = 9
// Version 3 = 9
#define CONTROL_SWITCH_PIN 9


#include "NeoPixel.h"

extern NeoPixel _neoPixel;
