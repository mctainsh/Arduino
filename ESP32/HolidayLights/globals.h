#pragma once

#define MY_VERSION "2.03"
// Maximum power is 255. 50 is a good value
//  88 = 2.9A on 308 LEDs
// 188 = 3.8A on 308 LEDssmart
//#define MAX_POWER	(127)

#define PIXEL_COUNT 108		// Home Bed
#define PIXEL_COUNT 300		// Long strip

// Use this if switching based non CPU
//#if defined(CONFIG_IDF_TARGET_ESP32)
//	#define NEOPIXEL_RGBW_PIN 6
//#elif defined(CONFIG_IDF_TARGET_ESP32S2)
//	#define NEOPIXEL_RGBW_PIN 6
//#elif defined(CONFIG_IDF_TARGET_ESP32C3)
//	#define NEOPIXEL_RGBW_PIN 6
//#endif

// Which input switch pin on the Arduino is connected to the NeoPixels?
// Version 1 = 2
// Version 2 = 1
// Version 3 = 6
// LOLIN S2 Mini = 18
#define NEOPIXEL_RGBW_PIN    6	

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

const char* MANUFACTURER = "Secure Hub";

// Make the following unique for each device (Bridge and paring setup code)
// Default Paring Code "46637726"
//const char* BRIDGE_NAME = "SH Bridge 01";		// Home bedroom LEDS = 108
//const char* PARING_CODE = "88880001";
//const char* BRIDGE_NAME = "SH Bridge 02";		// Flat floor bed 300 LEDS=300
//const char* PARING_CODE = "88880002";
//const char* BRIDGE_NAME = "SH Bridge 03";
//const char* PARING_CODE = "88880003";
//const char* BRIDGE_NAME = "SH Lego 01";		// Claudia's Lego #1
//const char* PARING_CODE = "88880010";
//const char* BRIDGE_NAME = "Stranger Things";	// Claudia's Lego #2 Stranger times
//const char* PARING_CODE = "88880011";
//const char* BRIDGE_NAME = "Farm Strip";		// Farm bed light
//const char* PARING_CODE = "88880012";
//const char* BRIDGE_NAME = "Lego Workshop";	// Lego workshop
//const char* PARING_CODE = "88880015";
//const char* SERIAL_NO	  = "JRM.008.3";		// Chimney
//const char* BRIDGE_NAME = "Chimney";			
//const char* PARING_CODE = "88880016";
//const char* SERIAL_NO   = "JRM.008.17";
//const char* BRIDGE_NAME = "KitchenCabinet";	// Kitchen Cabinet
//const char* PARING_CODE = "88880017";
const char* SERIAL_NO   = "JRM.008.19";
const char* BRIDGE_NAME = "Christmas Lights";	// Kitchen Cabinet
const char* PARING_CODE = "88880019";



//#include "NeoPixel.h"

//extern NeoPixel _neoPixel;
void TurnOnStrip(bool on);
