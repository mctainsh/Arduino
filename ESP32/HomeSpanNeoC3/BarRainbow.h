#include "globals.h"

extern SpanCharacteristic *_rainbowPower;
extern SpanCharacteristic *_plainPower;

struct BarRainbow : Service::LightBulb         // RGB LED (Command Cathode)
{	
	//SpanCharacteristic *power;                   // reference to the On Characteristic
	SpanCharacteristic *V;                       // reference to the Brightness Characteristic

	BarRainbow() : Service::LightBulb()
	{
		Serial.printf("Setting up Gay Strip\n");

		_neoPixel.Fill(0,50,50, 3);

		_rainbowPower=new Characteristic::On();
		V=new Characteristic::Brightness(50);     	// instantiate the Brightness Characteristic with an initial value of 100%
		V->setRange(1,100,1);                     	// sets the range of the Brightness to be from a min of 1%, to a max of 100%, in steps of 1%

		Serial.printf("Configured Gay LED\n");

	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Update from Apple
	boolean update() 
	{
		float v = 0;

		v=V->getVal<float>();                      // though H and S are defined as FLOAT in HAP, V (which is brightness) is defined as INT, but will be re-cast appropriately

		boolean powerOn=_rainbowPower->getVal();
		Serial.printf("INPUT  V=%.0f Pwr=%d \n", v, powerOn);

		// Get the new brightness
		if(V->updated())
		{
			v=V->getNewVal<float>();
			Serial.printf("New Brightness%.2f \n", v);
		}

		// Power setup
		if(_rainbowPower->updated())
		{
			powerOn=_rainbowPower->getNewVal();
			Serial.printf("New P:%d \n", powerOn);
			if( powerOn )
			{			
				_neoPixel.SetMode( NeoPixel::BarMode::rainbowBar);
				_plainPower->setVal(false);	
				_neoPixel.setBrightness((int)(MAX_POWER*(v/100.0)));
			}
			else
			{
				_neoPixel.Off();
				return(true);
			}
		}
		Serial.printf("Output V=%.0f Pwr=%d \n", v, powerOn);
		return(true);                               // return true

	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Background processing
	void loop()
	{
		// Only need to call loop once
		_neoPixel.Loop();
	}
};
