#include "esp32-hal.h"
#include "Arduino.h"
#include "Globals.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dimable and colour changing
struct DEV_RgbLED : DEV_Base
{
	SpanCharacteristic *H;	
	SpanCharacteristic *S;	

	DEV_RgbLED()
	  : DEV_Base()
	{
		H = new Characteristic::Hue(0);			// instantiate the Hue Characteristic with an initial value of 0 out of 360
		S = new Characteristic::Saturation(0);	// instantiate the Saturation Characteristic with an initial value of 0%
	}

///////////////////////////////////////////////////////////////////////////////////////
// Update the changed values
	boolean update()
	{
		UpdateBase();

		float h = H->getVal<float>();  // get and store all current values.  Note the use of the <float> template to properly read the values
		float s = S->getVal<float>();

		char cBuf[128];
		//	sprintf(cBuf, "Updating RGB LED: Pins=(%d,%d,%d): ", redPin->getPin(), greenPin->getPin(), bluePin->getPin());
		//	LOG1(cBuf);

		if (H->updated())
		{
			h = H->getNewVal<float>();
			sprintf(cBuf, "H=%.0f->%.0f, ", H->getVal<float>(), h);
		}
		else
		{
			h = H->getVal<float>();
			sprintf(cBuf, "H=%.0f, ", h);
		}
		LOG1(cBuf);

		if (S->updated())
		{
			s = S->getNewVal<float>();
			sprintf(cBuf, "S=%.0f->%.0f, ", S->getVal<float>(), s);
		}
		else
		{
			s = S->getVal<float>();
			sprintf(cBuf, "S=%.0f, ", s);
		}

		LOG1(cBuf);

		// Here we call a static function of LedPin that converts HSV to RGB.
		// Parameters must all be floats in range of H[0,360], S[0,1], and V[0,1]
		// R, G, B, returned [0,1] range as well

		float r, g, b;
		LedPin::HSVtoRGB(h, s / 100.0, _powerLevel / 100.0, &r, &g, &b);  // since HomeKit provides S and V in percent, scale down by 100
		//LedPin::HSVtoRGB(h, s / 100.0, 1, &r, &g, &b);	// since HomeKit provides S and V in percent, scale down by 100

		int R = r * 100;  // since LedPin uses percent, scale back up by 100, and multiple by status fo power (either 0 or 1)
		int G = g * 100;
		int B = b * 100;

		//	for (int n = 0; n < PIXEL_COUNT; n++)
		//		g_strip.setPixelColor(n, g_strip.Color(R, G, B));
		//	g_strip.show();


		sprintf(cBuf, "RGB=(%d,%d,%d)\n", R, G, B);
		LOG1(cBuf);


		return true;
	}
};
