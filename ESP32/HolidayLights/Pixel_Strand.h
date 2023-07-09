#include "Effects.h"

#define OVERFLOW_MILLIS_LIMIT (60 * 60 * 1000)

////////////////////////////////////////////////////////////////////////////////////////
// Addressable RGBW Pixel Strand of nPixel Pixels
struct Pixel_Strand : Service::LightBulb
{
	Characteristic::On _power{ 0, true };
	Characteristic::Hue H{ 0, true };
	Characteristic::Saturation S{ 100, true };
	Characteristic::Brightness V{ 100, true };
	Characteristic::Selector _effect{ 1, true };

	// Collection of effects
	vector<SpecialEffect *> Effects;

	// Time before the alarm activates again
	uint32_t _alarmTime;

	Pixel_Strand(Pixel *pixelIn, int nPixels)
	  : Service::LightBulb()
	{
		Effects.push_back(new ManualControl_1());
		Effects.push_back(new KnightRider_2());
		Effects.push_back(new Random_3());
		Effects.push_back(new Twinkle_4());
		Effects.push_back(new Rainbow_5());
		Effects.push_back(new RaceTrack_6());

		_effect.setUnit("");  // configures custom "Selector" characteristic for use with Eve HomeKit
		_effect.setDescription("Color Effect");
		_effect.setRange(1, Effects.size(), 1);

		V.setRange(1, 100, 1);	// sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%

		// Determine the biggest buffer size required
		int bufSize = 0;
		for (int i = 0; i < Effects.size(); i++)
			bufSize = Effects[i]->requiredBuffer() > bufSize ? Effects[i]->requiredBuffer() : bufSize;

		g_colors = (Pixel::Color *)calloc(bufSize, sizeof(Pixel::Color));  // storage for dynamic pixel pattern

		Serial.printf("\nConfigured Pixel_Strand on with %d pixels and %d effects.  Color buffer = %d pixels\n\n", nPixels, Effects.size(), bufSize);

		update();
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Update called when timer runs out
	boolean update() override
	{
		int32_t powerVal = _power.getNewVal();
		int32_t effectIndex = _effect.getNewVal();
		float h = H.getNewVal<float>();
		float s = S.getNewVal<float>();
		float v = V.getNewVal<float>();
		Serial.printf("Power:%d, H:%f, S:%f, V:%f Effect:%d\n", powerVal, h, s, v, effectIndex);
		if (!powerVal)
		{
			Serial.println("Power -> OFF");
			g_pixel.set(Pixel::Color().RGB(0, 0, 0, 0), PIXEL_COUNT);
			TurnOnStrip(false);
		}
		else
		{
			// Turn on the light strip
			TurnOnStrip(true);

			// Get the effect
			SpecialEffect *pEffect = Effects[effectIndex - 1];

			// Call Init on the effect
			pEffect->init(h, s, v);

			// Reset timer
			_alarmTime = 0;

			// If the effect changed, record the change
			if (_effect.updated())
				Serial.printf("Effect changed to: %s\n", pEffect->name);
		}
		return (true);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Loop runs until alarm time.
	void loop() override
	{
		// Do nothing if not turned on
		if (!_power.getVal())
			return;


		// Check if timer expired or overflow
		unsigned long t = millis();
		if (t > _alarmTime)
		{
			// Reload new effect after time out
			_alarmTime = t + Effects[_effect.getNewVal() - 1]->update();
		}
		else if (_alarmTime > OVERFLOW_MILLIS_LIMIT && t < (_alarmTime - OVERFLOW_MILLIS_LIMIT))
		{
			// Overflow check
			Serial.printf("WARNING !!! OVERFLOW %ld TRIGGERED!!! For %ld", OVERFLOW_MILLIS_LIMIT, _alarmTime);
			_alarmTime = t + Effects[_effect.getNewVal() - 1]->update();
		}
	}
};
