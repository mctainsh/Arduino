/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2022 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/

// HomeSpan Addressable RGB LED Example

// Add Homespan
// Add ESP32C3 Dev Module

// Demonstrates use of HomeSpan Pixel Class that provides for control of single-wire
// addressable RGB LEDs, such as the SK68xx or WS28xx models found in many devices,
// including the Espressif ESP32, ESP32-S2, and ESP32-C3 development boards.

// Also demonstrates how to take advantage of the Eve HomeKit App's ability to render
// a generic custom Characteristic.  The sketch uses a custom Characterstic to create
// a "selector" button that enables to the user to select which special effect to run

#define REQUIRED VERSION(1, 5, 0)

#include "HomeSpan.h"
//#include "extras/Pixel.h"  // include the HomeSpan Pixel class
#include <Adafruit_NeoPixel.h>
#include "globals.h"
#include "Effects.h"


boolean _powerOn = false;


// Declare our NeoPixel strip object:
Adafruit_NeoPixel g_strip(PIXEL_COUNT, NEOPIXEL_RGBW_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


/////////////////////////////////////////////////////////////////////////////////////////////
void TurnOnStrip(bool on)
{
	if (_powerOn == on)
		return;
	pinMode(STRIP_POWER, on ? OUTPUT : INPUT);

	Serial.println(on ? "Turn OFF" : "Turn OFF");

	if (on)
		digitalWrite(STRIP_POWER, HIGH);
	_powerOn = on;
}

void onSetConnection(int mode)
{
	Serial.printf("onSetConnection -> %d\r\n", mode);
	//	_pixel->set(Pixel::Color().RGB(255, 0, 0, 99), 1);
	//digitalWrite(STRIP_POWER, LOW);
	//pixel->off();
	//TurnOnStrip(false);
}

void setup()
{
	//Serial.begin(115200);
	Serial.printf("JRM:Starting V%s %s\n", MY_VERSION, PARING_CODE);

	// Show a few pixels
	Serial.printf("Activate strip on pin %d\n", NEOPIXEL_RGBW_PIN);
	TurnOnStrip(true);

	g_strip.begin();			  // INITIALIZE NeoPixel strip object (REQUIRED)
	g_strip.show();			  // Turn OFF all pixels ASAP
	g_strip.setBrightness(100);  // Set BRIGHTNESS to about 1/5 (max = 255)

	g_strip.setPixelColor(1, g_strip.Color(255, 0, 0));
	g_strip.show();

	// System detail
	Serial.printf("Internal heap\n");
	Serial.printf("  Total       %.2fKb\n", ESP.getHeapSize() / 1000.0);
	Serial.printf("  Free        %.2fkb\n", ESP.getFreeHeap() / 1000.0);
	Serial.printf("SPIRam\n");
	Serial.printf("  Total heap  %d\n", ESP.getPsramSize());
	Serial.printf("  Free Heap   %d\n", ESP.getFreePsram());
	Serial.printf("Flash\n");
	Serial.printf("  Sketch size %.2fMb\n", ESP.getSketchSize() / 1000000.0);
	Serial.printf("  Sketch free %.2fMb\n", ESP.getFreeSketchSpace() / 1000000.0);
	Serial.printf("  Size        %.2fMb\n", ESP.getFlashChipSize() / 1000000.0);
	Serial.printf("  Speed       %.2fMHz\n", ESP.getFlashChipSpeed() / 1000000.0);
	Serial.printf("  Mode        %d\n", ESP.getFlashChipMode());
	Serial.printf("  Cycles      %d\n", ESP.getCycleCount());
	Serial.printf("Chip\n");
	Serial.printf("  Cpu Freq    %dMHz\n", ESP.getCpuFreqMHz());
	Serial.printf("  Cores       %d\n", ESP.getChipCores());
	Serial.printf("  Model       %s\n", ESP.getChipModel());
	Serial.printf("  Revision    %d\n", ESP.getChipRevision());
	Serial.printf("  SDK Version %s\n", ESP.getSdkVersion());

	//Serial.printf("WIFI\n");
	//Serial.printf("  IP address  %s\n", WiFi.localIP());
	//Serial.printf("  Mac Address %s\n", Wifi.macAddress());
	//Serial.printf("  Subnet Mas  %s\n", WiFi.subnetMask());
	//Serial.printf("  Gateway IP  %s\n", WiFi.gatewayIP());
	//Serial.print( "  DNS         %s\n", WiFi.dnsIP());

	// Setup homespan
	g_strip.setPixelColor(1, g_strip.Color(0, 255, 0));
	g_strip.show();

	homeSpan.setConnectionCallback(onSetConnection);

	homeSpan.setStatusPin(STATUS_LED_PIN);		 // 9 Is blue, 10 is red
	homeSpan.setStatusAutoOff(30);				 // Turn off status LED after 30 seconds
	homeSpan.setControlPin(CONTROL_SWITCH_PIN);	 // 18 is nearest GND, 9 is PRG Button

	// Setup the parting code (Should be unique on the network)
	homeSpan.setPairingCode(PARING_CODE);

	// Setup to jump to WIFI setup if no WIFI config
	//homeSpan.setApPassword("homespan");
	homeSpan.enableAutoStartAP();
	//homeSpan.setWifiCredentials("RhinoNBN", "");

	homeSpan.begin(Category::Lighting, BRIDGE_NAME);

	// Setup the acessory
	g_strip.setPixelColor(2, g_strip.Color(0,0,255));
	g_strip.show();

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Name(BRIDGE_NAME);
	new Characteristic::Manufacturer(MANUFACTURER);
	new Characteristic::SerialNumber(SERIAL_NO);
	new Characteristic::Model("John McTainsh HolidayNeoPixel");
	new Characteristic::FirmwareRevision("1.0");
	new Characteristic::Identify();

	new Service::HAPProtocolInformation();
	new Characteristic::Version("1.1.0");


	// Complete
	g_strip.setPixelColor(3, g_strip.Color(255,0,255));
	g_strip.show();
}

Rainbow_5 rain = Rainbow_5();


void loop()
{
TurnOnStrip(true);
rain.update();
return;
	homeSpan.poll();
}
