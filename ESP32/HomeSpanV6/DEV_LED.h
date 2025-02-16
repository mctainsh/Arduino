#include "esp32-hal.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "Globals.h"

extern bool g_rainbowOn;

//////////////////////////////////
struct DEV_DimmableLED : Service::LightBulb
{  // Dimmable LED

	//LedPin *ledPin;				// reference to Led Pin
	SpanCharacteristic *power;	// reference to the On Characteristic
	SpanCharacteristic *level;	// reference to the Brightness Characteristic
	long _firstPixelHue;
	bool _powerOn = false;

	DEV_DimmableLED(int pin)
	  : Service::LightBulb()
	{  // constructor() method

		power = new Characteristic::On();

		level = new Characteristic::Brightness(50);	 // Brightness Characteristic with an initial value of 50%
		level->setRange(5, 100, 1);					 // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%

		//this->ledPin = new LedPin(pin);	 // configures a PWM LED for output to the specified pin

		Serial.print("Configuring Dimmable LED: Pin=");	 // initialization message
		//Serial.print(ledPin->getPin());
		Serial.print("\n");

	}  // end constructor

	boolean update()
	{

		TurnOnStrip(true);

		LOG1("Updating Dimmable LED on pin=");
		//LOG1(ledPin->getPin());
		LOG1(":  Current Power=");
		LOG1(power->getVal() ? "true" : "false");
		LOG1("  Current Brightness=");
		LOG1(level->getVal());

		if (power->updated())
		{
			LOG1("  New Power=");
			_powerOn = power->getNewVal();
			LOG1(_powerOn ? "true" : "false");
			TurnOnStrip(_powerOn);
			g_rainbowOn = _powerOn;
		}

		if (level->updated())
		{
			LOG1("  New Brightness=");
			LOG1(level->getNewVal());
			//g_strip.setBrightness(level->getNewVal());
		}



		LOG1("\n");

		//ledPin->set(power->getNewVal() * level->getNewVal());

		return true;  // return true
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dimable and colour changing
struct DEV_RgbLED : Service::LightBulb
{  // RGB LED (Command Cathode)

	//	LedPin *redPin, *greenPin, *bluePin;

	SpanCharacteristic *power;	// reference to the On Characteristic
	SpanCharacteristic *H;		// reference to the Hue Characteristic
	SpanCharacteristic *S;		// reference to the Saturation Characteristic
	SpanCharacteristic *V;		// reference to the Brightness Characteristic

	DEV_RgbLED(int red_pin, int green_pin, int blue_pin)
	  : Service::LightBulb()
	{  // constructor() method

		power = new Characteristic::On();
		H = new Characteristic::Hue(0);			  // instantiate the Hue Characteristic with an initial value of 0 out of 360
		S = new Characteristic::Saturation(0);	  // instantiate the Saturation Characteristic with an initial value of 0%
		V = new Characteristic::Brightness(100);  // instantiate the Brightness Characteristic with an initial value of 100%
		V->setRange(5, 100, 1);					  // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%

		//	this->redPin = new LedPin(red_pin);		 // configures a PWM LED for output to the RED pin
		//	this->greenPin = new LedPin(green_pin);	 // configures a PWM LED for output to the GREEN pin
		//	this->bluePin = new LedPin(blue_pin);	 // configures a PWM LED for output to the BLUE pin

		//	char cBuf[128];
		//	sprintf(cBuf, "Configuring RGB LED: Pins=(%d,%d,%d)\n", redPin->getPin(), greenPin->getPin(), bluePin->getPin());
		//	Serial.print(cBuf);
	}

	boolean update()
	{  // update() method

		boolean p;
		float v, h, s, r, g, b;


		h = H->getVal<float>();	 // get and store all current values.  Note the use of the <float> template to properly read the values
		s = S->getVal<float>();
		v = V->getVal<float>();	 // though H and S are defined as FLOAT in HAP, V (which is brightness) is defined as INT, but will be re-cast appropriately
		p = power->getVal();

		char cBuf[128];
		//	sprintf(cBuf, "Updating RGB LED: Pins=(%d,%d,%d): ", redPin->getPin(), greenPin->getPin(), bluePin->getPin());
		//	LOG1(cBuf);

		if (power->updated())
		{
			p = power->getNewVal();
			sprintf(cBuf, "Power=%s->%s, ", power->getVal() ? "true" : "false", p ? "true" : "false");
			TurnOnStrip(p);
		}
		else
		{
			sprintf(cBuf, "Power=%s, ", p ? "true" : "false");
		}
		LOG1(cBuf);

		if (H->updated())
		{
			h = H->getNewVal<float>();
			sprintf(cBuf, "H=%.0f->%.0f, ", H->getVal<float>(), h);
		}
		else
		{
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
			sprintf(cBuf, "S=%.0f, ", s);
		}
		LOG1(cBuf);

		if (V->updated())
		{
			v = V->getNewVal<float>();
			sprintf(cBuf, "V=%.0f->%.0f  ", V->getVal<float>(), v);
			//g_strip.setBrightness(V->getNewVal());
		}
		else
		{
			sprintf(cBuf, "V=%.0f  ", v);
		}
		LOG1(cBuf);

		// Here we call a static function of LedPin that converts HSV to RGB.
		// Parameters must all be floats in range of H[0,360], S[0,1], and V[0,1]
		// R, G, B, returned [0,1] range as well

		LedPin::HSVtoRGB(h, s / 100.0, v / 100.0, &r, &g, &b);	// since HomeKit provides S and V in percent, scale down by 100
		//LedPin::HSVtoRGB(h, s / 100.0, 1, &r, &g, &b);	// since HomeKit provides S and V in percent, scale down by 100

		int R, G, B;

		R = p * r * 100;  // since LedPin uses percent, scale back up by 100, and multiple by status fo power (either 0 or 1)
		G = p * g * 100;
		B = p * b * 100;

		for (int n = 0; n < PIXEL_COUNT; n++)
			g_strip.setPixelColor(n, g_strip.Color(R, G, B));
		g_strip.show();


		sprintf(cBuf, "RGB=(%d,%d,%d)\n", R, G, B);
		LOG1(cBuf);

		//	redPin->set(R);	 // update each ledPin with new values
		//	greenPin->set(G);
		//	bluePin->set(B);

		return true;
	}
};
