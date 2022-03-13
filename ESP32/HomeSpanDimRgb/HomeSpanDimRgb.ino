/*********************************************************************************
 *  MIT License
 *
 *  Copyright (c) 2020 Gregg E. Berman
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

////////////////////////////////////////////////////////////
//                                                        //
//    HomeSpan: A HomeKit implementation for the ESP32    //
//    ------------------------------------------------    //
//                                                        //
// Example 10: Controlling a full-color RGB LED           //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h"
//#include "DEV_LED.h"
#include "DEV_RgbLED.h"
#include "DEV_Identify.h"
#include "globals.h"

const char* SERIAL_NO = "JRM.008.1";
const char* MANUFACTURER = "Secure Hub";

void onWifiLoaded();

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start the main load
void setup()
{
	_display.Setup();
	_neoPixel.Setup();
	_neoPixel.Fill(255,0,0,1);

	homeSpan.setWifiCallback(onWifiLoaded);
	
	// Example 10 illustrates how to control an RGB LED to set any color and brightness.
	// The configuration below should look familiar by now.  We've created a new derived Service,
	// call RgbLED to house all the required logic.  You'll find all the code in DEV_LED.h.
	// For completeness, this configuration also contains an on/off LED and a dimmable LED as shown
	// in prior examples.

	Serial.begin(115200);

	_display.Show("Setup bridges");
	homeSpan.begin(Category::Bridges,"Secure Hub Bridge");


	new SpanAccessory();
	new DEV_Identify("Bridge #1",MANUFACTURER,SERIAL_NO,"HS Bridge","0.9",3);
	new Service::HAPProtocolInformation();
	new Characteristic::Version("1.1.0");

	//new SpanAccessory();
	//new DEV_Identify("On/Off LED",MANUFACTURER,SERIAL_NO,"20mA LED","0.9",0);
	//new DEV_LED(16);                                                               // Create an On/Off LED attached to pin 16

	//new SpanAccessory();
	//new DEV_Identify("Dimmable LED",MANUFACTURER,SERIAL_NO,"20mA LED","0.9",0);
	//new DEV_DimmableLED(17);                                                       // Create a Dimmable (PWM-driven) LED using attached to pin 17

	new SpanAccessory();
	new DEV_Identify("Strip",MANUFACTURER,SERIAL_NO,"TTGO NeoPixel","0.9",0);
	new DEV_RgbLED();                                                      // Create an RGB LED attached to pins 32,22,23 (for R, G, and B LED anodes)

	_display.Show("Loaded");

} // end of setup()

//////////////////////////////////////

void loop()
{
	homeSpan.poll();
} 


void onWifiLoaded()
{
	_neoPixel.Off();
	_display.Wifi = true;
	_display.Refresh();
}
