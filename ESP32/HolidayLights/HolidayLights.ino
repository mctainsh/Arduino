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

// Demonstrates use of HomeSpan Pixel Class that provides for control of single-wire
// addressable RGB LEDs, such as the SK68xx or WS28xx models found in many devices,
// including the Espressif ESP32, ESP32-S2, and ESP32-C3 development boards.

// Also demonstrates how to take advantage of the Eve HomeKit App's ability to render
// a generic custom Characteristic.  The sketch uses a custom Characterstic to create
// a "selector" button that enables to the user to select which special effect to run

#define REQUIRED VERSION(1, 5, 0)

#include "HomeSpan.h"
#include "extras/Pixel.h"  // include the HomeSpan Pixel class
#include "globals.h"

#include "Pixel_Strand.h"

boolean _powerOn = false;

// Neopixel strip 
Pixel g_pixel = Pixel(NEOPIXEL_RGBW_PIN, false);

// Colour for each pixel on the strip (calloc'ed) later
Pixel::Color *g_colors;


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

void onWifiLoaded()
{
	//	_pixel->set(Pixel::Color().RGB(255, 0, 0, 99), 1);
	//digitalWrite(STRIP_POWER, LOW);
	//pixel->off();
	//TurnOnStrip(false);
}

void setup()
{
	// Define colour map


	// Allocation the memory for status pixel display
	const int32_t STATUS_PIXELS = 5;
	Pixel::Color *colors = (Pixel::Color *)calloc(STATUS_PIXELS, sizeof(Pixel::Color));
	colors[0].RGB(255, 0, 0);
	colors[1].RGB(0, 0, 255);
	colors[2].RGB(255, 0, 255);
	colors[3].RGB(0, 255, 0);

	Serial.begin(115200);
	Serial.printf("JRM:Starting V%s %s\n", MY_VERSION, PARING_CODE);

	// Show a few pixels
	Serial.printf("Activate strip on pin %d\n", NEOPIXEL_RGBW_PIN);
	TurnOnStrip(true);
	g_pixel.set(colors, 1);

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
	g_pixel.set(colors, 2);
	homeSpan.setWifiCallback(onWifiLoaded);

	homeSpan.setStatusPin(STATUS_LED_PIN);		 // 9 Is blue, 10 is red
	homeSpan.setStatusAutoOff(30);				 // Turn off staqtus LED after 30 seconds
	homeSpan.setControlPin(CONTROL_SWITCH_PIN);	 // 18 is nearest GND, 9 is PRG Button

	// Setup the parting code (Should be unique on the network)
	homeSpan.setPairingCode(PARING_CODE);

	// Setup to jump to WIFI setup if no WIFI config
	homeSpan.enableAutoStartAP();
	//homeSpan.setWifiCredentials("RhinoNBN", "##########");

	homeSpan.begin(Category::Lighting, BRIDGE_NAME);

	// Setup the acessory
	g_pixel.set(colors, 3);
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

	new Pixel_Strand(&g_pixel, PIXEL_COUNT);

	// Complete
	g_pixel.set(colors, 4);
}

void loop()
{
	homeSpan.poll();
}
