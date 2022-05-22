#include "globals.h"

extern SpanCharacteristic *_plainPower;

struct BarPlain : Service::LightBulb         // RGB LED (Command Cathode)
{
	//SpanCharacteristic *power;                 // reference to the On Characteristic
	SpanCharacteristic *H;                       // reference to the Hue Characteristic
	SpanCharacteristic *S;                       // reference to the Saturation Characteristic
	SpanCharacteristic *V;                       // reference to the Brightness Characteristic

	boolean _powerOn = false;

	BarPlain() : Service::LightBulb()
	{
		Serial.printf("Setting up Plain RGB Strip.\n");

		_neoPixel.Fill(0,0,50, 2);

		_plainPower=new Characteristic::On();
		H=new Characteristic::Hue(0);         		// instantiate the Hue Characteristic with an initial value of 0 out of 360
		S=new Characteristic::Saturation(0);   		// instantiate the Saturation Characteristic with an initial value of 0%
		V=new Characteristic::Brightness(100);     	// instantiate the Brightness Characteristic with an initial value of 100%
		V->setRange(1,100,1);                     	// sets the range of the Brightness to be from a min of 1%, to a max of 100%, in steps of 1%

		Serial.printf("Configured RGB LED\n");

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Update from Apple
	boolean update()                          // update() method
	{
		float v = 0;
		float h = 0;
		float s = 0;

		h=H->getVal<float>();                      // get and store all current values.  Note the use of the <float> template to properly read the values
		s=S->getVal<float>();
		v=V->getVal<float>();                      // though H and S are defined as FLOAT in HAP, V (which is brightness) is defined as INT, but will be re-cast appropriately

		_powerOn=_plainPower->getVal();
		Serial.printf("Original PLAIN H=%.0f S=%.0f V=%.0f Pwr=%d \n", h, s, v, _powerOn);

		// Get the new brightness
		if(V->updated())
		{
			v=V->getNewVal<float>();
			Serial.printf("New PLAIN Brightness%.2f \n", v);
		}

		// Power setup
		if(_plainPower->updated())
		{
			_powerOn=_plainPower->getNewVal();
			Serial.printf("New PLAIN P:%d \n", _powerOn);

			// TODO : Add minimum brightness check
			if( _powerOn )
			{
				// Activate the bar
				_neoPixel.SetMode( NeoPixel::BarMode::plain);
				// v is zero to 100. NeoPixel->Brightness is 0 to 255. 50 is quite bright
				//_neoPixel.setBrightness(MAX_POWER);
				_neoPixel.setBrightness((int)(MAX_POWER*(v/100.0)));
			}
			else
			{
				_neoPixel.Off();
			}
		}

		// Colour setup
		if(H->updated())
			h=H->getNewVal<float>();
		if(S->updated())
			s=S->getNewVal<float>();

		// Apply the new colour or power
		//_neoPixel.FillHSV(h/360.0*65535,s/100.0*255, v/100.0*255);
		int hue = h/360.0*65535;
		int sat = s/100.0*255;
		if( v < 10 )
		{ 			
			_neoPixel.FillSkipping(hue,sat,255, v, 10-v );
		}
		else
		{
			_neoPixel.FillHSV(hue,sat,255);
		}
		Serial.printf("Output H=%.0f S=%.0f V=%.0f Pwr=%d \n", h, s, v, _powerOn);
		return(true);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Background processing
	void loop()
	{
		// Only need to call loop once
		//_neoPixel.Wave();
		//_neoPixel.Wipe();
		//_neoPixel.RainbowLoop();
	}
};
