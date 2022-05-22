#pragma once

// Maximum power is 255. 50 is a good value
#define MAX_POWER	(127)

//#define LED_COUNT 108		// Home Bed
#define LED_COUNT 300		// Long strip

// Which input switch pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    2		// WAS 6

// Red LED Pin to show status  9 Is blue, 10 is red
#define STATUS_LED_PIN 10
// Red LED Pin to show status  9 Is blue, 10 is red
#define CONTROL_SWITCH_PIN 9


#include "NeoPixel.h"

extern NeoPixel _neoPixel;
