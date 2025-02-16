/*********************************************************************************
   Setup notes
 	1) Increase Partition size with Tools-> Partition size->Minimal Spifs
	2) Clear memory. 
 ********************************************************************************/


#include <Adafruit_NeoPixel.h>
#include "HomeSpan.h"
#include "DEV_LED.h"
#include "Globals.h"

Adafruit_NeoPixel g_strip(PIXEL_COUNT, NEOPIXEL_RGBW_PIN, NEO_GRB + NEO_KHZ800);

boolean _powerOn = false;
bool g_rainbowOn = false;

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

	// Dimable rainbow
	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();
	new Characteristic::Name("Rainbow LED");
	new DEV_DimmableLED(3);	 // Create a Dimmable (PWM-driven) LED using attached to pin 17

	Set(4, 255, 255, 0);

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();
	new Characteristic::Name("Light strip");
	new DEV_RgbLED(10, 11, 12);	 // Create an RGB LED attached to pins 32,22,23 (for R, G, and B LED anodes)

	Set(5, 255, 0, 255);
}
//////////////////////////////////////

void loop()
{
	homeSpan.poll();

	if (g_rainbowOn)
	{

		//				_firstPixelHue ++;
		//		_firstPixelHue = millis() / 15;
		//	g_strip.rainbow(_firstPixelHue);
		//	g_strip.show();

		int startHue = millis()*5;
		g_strip.rainbow(startHue);

		//	g_strip.show();
		//	for (int i = 0; i < PIXEL_COUNT; i++)
		//g_colors[i].HSV(startHue + i, 100, _v);
		//		g_strip.setPixelColor(i, g_strip.ColorHSV(startHue + i, 100, 100));
		g_strip.show();
	}

	if (millis() % 1000 > 500)
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
