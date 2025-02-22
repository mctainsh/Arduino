#pragma once

#include "esp32-hal.h"
#include "Arduino.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dimable and colour changing
struct DEV_Base : Service::LightBulb
{
	SpanCharacteristic *power;	// reference to the On Characteristic
	SpanCharacteristic *V;		// reference to the Brightness Characteristic

	bool _powerOn = false;
	float _powerLevel = 0;

	unsigned long _timeOfPowerChange;
	bool _changeComplete = true;

	DEV_Base()
	  : Service::LightBulb()
	{
		power = new Characteristic::On();
		V = new Characteristic::Brightness(100);  // instantiate the Brightness Characteristic with an initial value of 100%
		V->setRange(5, 100, 1);					  // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
	}

	bool PoweringOn()
	{
		return _powerOn && !_changeComplete;
	}

	void ResetPowerDown()
	{
		_changeComplete = true;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Get the powerlevel for the
	void SetPowerlevel()
	{
		// Has a change started
		if (_changeComplete)
			return;

		TurnOnStrip(true);

		unsigned long age = millis() - _timeOfPowerChange;

		const int MAX_BRIGHTNESS = 80;

		// Apply the new level value
		float brightness;
		if (_powerOn)
			brightness = _min(_powerLevel, MAX_BRIGHTNESS * (age / POWER_FADE_MS));
		else
			brightness = _max(0, MAX_BRIGHTNESS - MAX_BRIGHTNESS * (age / POWER_FADE_MS));

		g_strip.setBrightness(brightness);

		// Is the change complete
		if (age > POWER_FADE_MS)
		{
			_changeComplete = true;
			TurnOnStrip(_powerOn);
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Read the base setting
	void UpdateBase()
	{
		if (power->updated())
		{
			_powerOn = power->getNewVal();
			_timeOfPowerChange = millis();
			_changeComplete = false;
		}

		if (V->updated())
		{
			_powerLevel = V->getNewVal();
			_changeComplete = false;
		}
	}
};
