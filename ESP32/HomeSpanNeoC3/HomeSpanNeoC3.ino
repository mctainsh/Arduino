////////////////////////////////////////////////////////////
//                                                        //
//    HomeSpan: A HomeKit implementation for the ESP32    //
//    ------------------------------------------------    //
//                                                        //
// Example 10: Controlling a full-color RGB LED           //
// Setup https://github.com/HomeSpan/HomeSpan			  //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h"
#include "BarPlain.h"
#include "BarRainbow.h"
#include "DEV_Identify.h"
#include "globals.h"

const char* SERIAL_NO = "JRM.008.2";
const char* MANUFACTURER = "Secure Hub";

// Make the following unique for each device (Bridge and paring setup code)
// Default Paring Code "46637726"
const char* BRIDGE_NAME = "SH Bridge 01";	// Home bedroom LEDS = 108
const char* PARING_CODE = "88880001";
//const char* BRIDGE_NAME = "SH Bridge 02";	// Flat floor bed 300 LEDS=300
//const char* PARING_CODE = "88880002";
//const char* BRIDGE_NAME = "SH Bridge 03";
//const char* PARING_CODE = "88880003";

SpanCharacteristic *_rainbowPower;
SpanCharacteristic *_plainPower;


void onWifiLoaded();

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start the main load
void setup()
{
	//_display.Setup();
	_neoPixel.Setup();
	_neoPixel.Fill(255,0,0,1);

	homeSpan.setWifiCallback(onWifiLoaded);

	// Example 10 illustrates how to control an RGB LED to set any color and brightness.
	// The configuration below should look familiar by now.  We've created a new derived Service,
	// call RgbLED to house all the required logic.  You'll find all the code in DEV_LED.h.
	// For completeness, this configuration also contains an on/off LED and a dimmable LED as shown
	// in prior examples.

	Serial.begin(115200);

	Serial.printf("JRM:Starting V1.02 %s\n", PARING_CODE);

	// Setup control and display pins
	homeSpan.setStatusPin(10);		// 9 Is blue, 10 is red
	homeSpan.setStatusAutoOff(30);	// Turn off staqtus LED after 30 seconds
	homeSpan.setControlPin(9);		// 18 is nearest GND, 9 is PRG Button

	// Setup the parting code (Should be unique on the network)
	homeSpan.setPairingCode(PARING_CODE);

	// Setup to jump to WIFI setup if no WIFI config
	homeSpan.enableAutoStartAP();

	// Start homespan
	homeSpan.begin(Category::Bridges,BRIDGE_NAME);

	new SpanAccessory();
		new DEV_Identify("Bridge #1",MANUFACTURER,SERIAL_NO,"HS Bridge","0.9",3);
		new Service::HAPProtocolInformation();
		new Characteristic::Version("1.1.0");

	new SpanAccessory(false);
		new DEV_Identify("Plain Strip",MANUFACTURER,SERIAL_NO,"TTGO NeoPixel","0.9",0);
		new BarPlain();

	new SpanAccessory();
		new DEV_Identify("Gay Strip",MANUFACTURER,SERIAL_NO,"TTGO NeoPixel","0.9",0);
		new BarRainbow();

	Serial.printf("JRM:Status LED:%d\n", homeSpan.getStatusPin() );

} // end of setup()

//////////////////////////////////////

void loop()
{
	homeSpan.poll();
}


void onWifiLoaded()
{
	_neoPixel.Off();
}
