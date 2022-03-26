#include "globals.h"
#include "extras/PwmPin.h"                     	// library only used for HSV to RGB conversion


struct DEV_RgbLED : Service::LightBulb         // RGB LED (Command Cathode)
{
	boolean _isGay = false;
	SpanCharacteristic *power;                   // reference to the On Characteristic
	SpanCharacteristic *H;                       // reference to the Hue Characteristic
	SpanCharacteristic *S;                       // reference to the Saturation Characteristic
	SpanCharacteristic *V;                       // reference to the Brightness Characteristic

	boolean _powerOn = false;

	DEV_RgbLED( boolean isGay ) : Service::LightBulb()
	{
		_isGay = isGay;
		Serial.printf("Setting up RGB Strip. Gay %d\n", _isGay);

		_neoPixel.Fill(0,0,50, 2);

		power=new Characteristic::On();
		H=new Characteristic::Hue(0);         		// instantiate the Hue Characteristic with an initial value of 0 out of 360
		S=new Characteristic::Saturation(100);   	// instantiate the Saturation Characteristic with an initial value of 0%
		V=new Characteristic::Brightness(25);     	// instantiate the Brightness Characteristic with an initial value of 100%
		V->setRange(1,100,1);                     	// sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%

		Serial.printf("Configured RGB LED\n");

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Update from Apple
	boolean update()                          // update() method
	{
		float v, h, s, r, g, b;

		h=H->getVal<float>();                      // get and store all current values.  Note the use of the <float> template to properly read the values
		s=S->getVal<float>();
		v=V->getVal<float>();                      // though H and S are defined as FLOAT in HAP, V (which is brightness) is defined as INT, but will be re-cast appropriately
		_powerOn=power->getVal();

		// Get the new brightness
		if(V->updated())
		{
			v=V->getNewVal<float>();
			Serial.printf("New Brightness%.2f \n", s);
		}
		//_display.Brightness = v;

		// Power setup
		if(power->updated())
		{
			_powerOn=power->getNewVal();
			Serial.printf("Power %df \n", power);
			
			// TODO : Add minimum brightness check
			if( _powerOn )
			{
				// v is zero to 100. NeoPixel->Brightness is 0 to 255. 50 is quite bright
				_neoPixel.setBrightness(50);
			}
			else
			{
				_neoPixel.Off();
				//_display.Refresh();
				return(true);
			}
		}

		// Colour setup
		if(H->updated())
		{
			h=H->getNewVal<float>();
		}

		if(S->updated())
		{
			s=S->getNewVal<float>();
		}

		// Here we call a static function of LedPin that converts HSV to RGB.
		// Parameters must all be floats in range of H[0,360], S[0,1], and V[0,1]
		// R, G, B, returned [0,1] range as well
		LedPin::HSVtoRGB(h,s/100.0,100.0,&r,&g,&b);   // since HomeKit provides S and V in percent, scale down by 100

		// Apply the new colour
		_neoPixel.FillHSV(h/360.0*65535,s/100.0*255,v/100.0*255);
		_neoPixel.show();
		Serial.printf("Output HSV=(%.2f,%.2f) V(Brightness)=%.2f  Pwr=%d \n", h, s, v, _powerOn);
		//_display.R = r*255;
		//_display.G = g*255;
		//_display.B = b*255;
		//_display.Refresh();
		return(true);                               // return true

	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Background processing
	void loop()
	{
		if( !_powerOn )
			return;


		// If powered on do something with the lights
		if( _isGay )
			_neoPixel.Rainbow();
		//else
		//	_neoPixel.Wave();
	}
};
