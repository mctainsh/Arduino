#pragma once

// NOTE : Accessa Point Password "homespan"

///////////////////////////////////////////////////////////////////////////////
// Setting up lights
//
// 1) Set pixel Fromat
//		Select WS2812 (3 pin,  5V) GRB
// 		Select WS2815 (4 pin, 12V) RGB
//
//	2) Setup max brightness by limiting Effect.h (_v = v * x) 0.25 -> 2.50
//		Select WS2812 (3 pin) - x = 1.00
// 		Select WS2815 (4 pin) - x = 0.50
//
//	3) Bridge details
//
//	4) Number of pixels


#define NEO_RGB ((0 << 6) | (0 << 4) | (1 << 2) | (2)) ///< Transmit as R,G,B
//#define NEO_RBG ((0 << 6) | (0 << 4) | (2 << 2) | (1)) ///< Transmit as R,B,G
//#define NEO_GRB ((1 << 6) | (1 << 4) | (0 << 2) | (2)) ///< Transmit as G,R,B
//#define NEO_GBR ((2 << 6) | (2 << 4) | (0 << 2) | (1)) ///< Transmit as G,B,R
//#define NEO_BRG ((1 << 6) | (1 << 4) | (2 << 2) | (0)) ///< Transmit as B,R,G
//#define NEO_BGR ((2 << 6) | (2 << 4) | (1 << 2) | (0)) ///< Transmit as B,G,R

#define MY_VERSION "2.38"
// Maximum power is 255. 50 is a good value
//  88 = 2.9A on 308 LEDs
// 188 = 3.8A on 308 LEDssmart
//#define MAX_POWER	(127)

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
//#define PIXEL_COUNT 108		
//const char* BRIDGE_NAME = "SH Bridge 02";		// Flat floor bed 300 LEDS=300
//const char* PARING_CODE = "88880002";
//const char* BRIDGE_NAME = "SH Bridge 03";
//const char* PARING_CODE = "88880003";
//const char* BRIDGE_NAME = "SH Lego 01";		// Claudia's Lego #1
//const char* PARING_CODE = "88880010";
//const char* BRIDGE_NAME = "Stranger Things";	// Claudia's Lego #2 Stranger times
//const char* PARING_CODE = "88880011";
//const char* BRIDGE_NAME = "Farm Strip";		// Farm bed head light
//const char* PARING_CODE = "88880012";
//const char* BRIDGE_NAME = "Lego Workshop";	// Lego workshop
//const char* PARING_CODE = "88880015";
//const char* SERIAL_NO	  = "JRM.008.3";		// Chimney
//const char* BRIDGE_NAME = "Chimney";			
//const char* PARING_CODE = "88880016";
//const char* SERIAL_NO   = "JRM.008.17";
//const char* BRIDGE_NAME = "KitchenCabinet";	// Kitchen Cabinet
//const char* PARING_CODE = "88880017";
//const char* SERIAL_NO   = "JRM.008.19";		// Master Bedroom Top Lights
//const char* BRIDGE_NAME = "Christmas Lights";	
//const char* PARING_CODE = "88880019";
//const char* SERIAL_NO   = "JRM.008.20";			// Farm Master bedroom
//const char* BRIDGE_NAME = "Farm Room Lights";	
//const char* PARING_CODE = "88880020";
const char* SERIAL_NO   = "JRM.008.21";			// Claudia's TV
const char* BRIDGE_NAME = "Claudias TV Lights";	
const char* PARING_CODE = "88880021";

#define PIXEL_COUNT 300
// Claudia TV = 135		

// The Neopixel controller
extern Pixel g_pixel;

// Array for pixels to fill with colour
extern Pixel::Color *g_colors;

// Turn on pixel
void TurnOnStrip(bool on);

CUSTOM_CHAR(Selector, 88888888-D0C1-4CFA-8D00-B044D1E9E989, PR + PW + EV, UINT8, 1, 1, 6, false);  // create Custom Characteristic to "select" special effects via Eve App
//CUSTOM_CHAR(Selector, 00000001-0001-0001-0001-46637266EA00, PR + PW + EV, UINT8, 1, 1, 5, false);  // create Custom Characteristic to "select" special effects via Eve App
