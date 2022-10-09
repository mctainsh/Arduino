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
#include "DEV_Identify.h"
#include "globals.h"

const char* SERIAL_NO = "JRM.008.3";
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
//const char* BRIDGE_NAME = "Farm Strip";			// Farm bed light
//const char* PARING_CODE = "88880012";
const char* BRIDGE_NAME = "Lego Workshop";		// Lego workshop
const char* PARING_CODE = "88880015";

SpanCharacteristic *_plainPower;

void onWifiLoaded();

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start the main load
void setup()
{
	_neoPixel.Setup();
	_neoPixel.Fill(255,0,0,1);

	homeSpan.setWifiCallback(onWifiLoaded);

	Serial.begin(115200);

	Serial.printf("JRM:Starting V1.28 %s\n", PARING_CODE);

	// Setup control and display pins
	pinMode(STRIP_POWER, OUTPUT);
	digitalWrite(STRIP_POWER, HIGH);

	homeSpan.setStatusPin(STATUS_LED_PIN);			// 9 Is blue, 10 is red
	homeSpan.setStatusAutoOff(30);					// Turn off staqtus LED after 30 seconds
	homeSpan.setControlPin(CONTROL_SWITCH_PIN);		// 18 is nearest GND, 9 is PRG Button

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
		new DEV_Identify("Strip",MANUFACTURER,SERIAL_NO,"TTGO NeoPixel","0.9",0);
		new BarPlain();

	Serial.printf("JRM:Status LED:%d\n", homeSpan.getStatusPin() );

} // end of setup()

//////////////////////////////////////

void loop()
{
	homeSpan.poll();
}


void onWifiLoaded()
{
	digitalWrite(STRIP_POWER, LOW);
	_neoPixel.Off();
}
