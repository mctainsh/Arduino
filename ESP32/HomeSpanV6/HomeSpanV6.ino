/*********************************************************************************
   Setup notes
 	1) Increase Partition size with Tools -> Partition size -> Minimal Spiffs (Or Huge APP)
	2) Clear memory (ONLY IF NEW). Tools -> Erase All Flast before upload -> Enable 
	3) Set Library versions
		HomeSpan 2.1.0
		Adafruit_NeoPixel 1.12.4
 ********************************************************************************/


#include <Adafruit_NeoPixel.h>
#include "HomeSpan.h"
#include "DEV_DimmableLED.h"
#include "DEV_RgbLED.h"
#include "Globals.h"

Adafruit_NeoPixel g_strip(PIXEL_COUNT, NEOPIXEL_RGBW_PIN, NEO_GRB + NEO_KHZ800);

boolean _powerOn = false;

DEV_DimmableLED* _pRainbowStrip = NULL;
DEV_RgbLED* _pRgbStrip = NULL;

void Set(int pin, int r, int g, int b)
{
	g_strip.setPixelColor(pin, g_strip.Color(r, g, b));
	g_strip.show();
}

void setup()
{
	Serial.begin(115200);

	TurnOnStrip(true);
	g_strip.begin();			 // INITIALIZE NeoPixel strip object (REQUIRED)
	g_strip.show();				 // Turn OFF all pixels ASAP
	g_strip.setBrightness(100);	 // Set BRIGHTNESS to about 1/5 (max = 255)

	Set(1, 255, 0, 0);

	homeSpan.begin(Category::Bridges, BRIDGE_NAME);
	homeSpan.setWifiCredentials("RhinoNBN", "");

	homeSpan.setStatusPin(STATUS_LED_PIN);	// 9 Is blue, 10 is red
	homeSpan.setStatusAutoOff(30);			// Turn off status LED after 30 seconds
	//homeSpan.setControlPin(CONTROL_SWITCH_PIN);	 // 18 is nearest GND, 9 is PRG Button

	// Setup the parting code (Should be unique on the network)
	//homeSpan.setPairingCode(PARING_CODE);

	Set(2, 0, 255, 0);

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();

	Set(3, 0, 0, 255);

	// Create a Dimmable Raunbow strip
	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();
	new Characteristic::Name("Rainbow LED");
	_pRainbowStrip = new DEV_DimmableLED();

	Set(4, 255, 255, 0);

	// Create simple colour strip
	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();
	new Characteristic::Name("Light strip");
	_pRgbStrip = new DEV_RgbLED();

	Set(5, 255, 0, 255);
}
//////////////////////////////////////

void loop()
{
	homeSpan.poll();


	// If one is powering on and one off
	// .. Cancel the power off and accept power ON

	// Chase the rainbow
	if (_pRainbowStrip != NULL && _pRainbowStrip->_powerOn)
	{
		int startHue = millis() * 5;
		g_strip.rainbow(startHue);
		g_strip.show();
	}

	// If stuff is still pending blink
	if (millis() % 500 > 250)
		Set(0, 255, 0, 255);
	else
		Set(0, 0, 0, 255);
}

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
