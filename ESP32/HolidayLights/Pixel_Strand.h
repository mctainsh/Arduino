
CUSTOM_CHAR(Selector, 88888888-D0C1-4CFA-8D00-B044D1E9E989, PR + PW + EV, UINT8, 1, 1, 5, false);  // create Custom Characteristic to "select" special effects via Eve App
//CUSTOM_CHAR(Selector, 00000001-0001-0001-0001-46637266EA00, PR + PW + EV, UINT8, 1, 1, 5, false);  // create Custom Characteristic to "select" special effects via Eve App

#define OVERFLOW_MILLIS_LIMIT (10 * 1000)

////////////////////////////////////////////////////////////////////////////////////////
// Addressable RGBW Pixel Strand of nPixel Pixels
struct Pixel_Strand : Service::LightBulb
{
	Characteristic::On power{ 0, true };
	Characteristic::Hue H{ 0, true };
	Characteristic::Saturation S{ 100, true };
	Characteristic::Brightness V{ 100, true };
	Characteristic::Selector effect{ 1, true };

	///////////////////////////////////////////////////////////////////////////////////
	// Effect base structure
	struct SpecialEffect
	{
		Pixel_Strand *px;
		const char *name;

		SpecialEffect(Pixel_Strand *px, const char *name)
		{
			this->px = px;
			this->name = name;
			Serial.printf("Adding Effect %d: %s\n", px->Effects.size() + 1, name);
		}

		virtual void init() {}
		virtual uint32_t update()
		{
			return (60000);
		}
		virtual int requiredBuffer()
		{
			return (0);
		}
	};

	// Collection of effects
	vector<SpecialEffect *> Effects;

	int nPixels;
	Pixel *pixel;
	Pixel::Color *colors;

	// Time before the alarm activates again
	uint32_t _alarmTime;

	Pixel_Strand(Pixel *pixelIn, int nPixels)
	  : Service::LightBulb()
	{
		pixel = pixelIn;
		//pixel = new Pixel(NEOPIXEL_RGBW_PIN, true);  // creates RGBW pixel LED on specified pin using default timing parameters suitable for most SK68xx LEDs
		this->nPixels = nPixels;	   // store number of Pixels in Strand

		Effects.push_back(new ManualControl1(this));
		Effects.push_back(new KnightRider2(this));
		Effects.push_back(new Random3(this));
		Effects.push_back(new Twinkle4(this));
		Effects.push_back(new RaceTrack5(this));

		effect.setUnit("");	 // configures custom "Selector" characteristic for use with Eve HomeKit
		effect.setDescription("Color Effect");
		effect.setRange(1, Effects.size(), 1);

		V.setRange(5, 100, 1);	// sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%

		int bufSize = 0;

		for (int i = 0; i < Effects.size(); i++)
			bufSize = Effects[i]->requiredBuffer() > bufSize ? Effects[i]->requiredBuffer() : bufSize;

		colors = (Pixel::Color *)calloc(bufSize, sizeof(Pixel::Color));	 // storage for dynamic pixel pattern

		Serial.printf("\nConfigured Pixel_Strand on with %d pixels and %d effects.  Color buffer = %d pixels\n\n", nPixels, Effects.size(), bufSize);

		update();
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Update called when timer runs out
	boolean update() override
	{
		if (!power.getNewVal())
		{
			Serial.println("Power -> OFF");
			pixel->set(Pixel::Color().RGB(0, 0, 0, 0), nPixels);
			TurnOnStrip(false);
		}
		else
		{
			// Turn on the light strip
			TurnOnStrip(true);

			// Get the effect
			SpecialEffect *pEffect = Effects[effect.getNewVal() - 1];

			// Call Init on the effect
			pEffect->init();

			// Reset timer
			_alarmTime = 0;

			// If the effect changed, record the change
			if (effect.updated())
				Serial.printf("Effect changed to: %s\n", pEffect->name);
		}
		return (true);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Loop runs until alarm time.
	void loop() override
	{
		// Do nothing if not turned on
		if (!power.getVal())
			return;


		// Check if timer expired or overflow
		unsigned long t = millis();
		if (t > _alarmTime)
		{
			_alarmTime = t + Effects[effect.getNewVal() - 1]->update();
		}
		else if (_alarmTime > OVERFLOW_MILLIS_LIMIT && t < (_alarmTime - OVERFLOW_MILLIS_LIMIT))
		{
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			Serial.println("WARNING !!! OVERFLOW TRIGGERED!!!");
			_alarmTime = t + Effects[effect.getNewVal() - 1]->update();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	struct ManualControl1 : SpecialEffect
	{
		ManualControl1(Pixel_Strand *px) : SpecialEffect{ px, "Manual Control" } {}

		void init() override
		{
			TurnOnStrip(true);
			px->pixel->set(Pixel::Color().HSV(px->H.getNewVal<float>(), 
				px->S.getNewVal<float>(), 
				px->V.getNewVal<float>()), 
				px->nPixels);
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////
	struct KnightRider2 : SpecialEffect
	{
		int phase = 0;
		int dir = 1;

		KnightRider2(Pixel_Strand *px) : SpecialEffect{ px, "KnightRider" } {}

		void init() override
		{
			float level = px->V.getNewVal<float>();
			for (int i = 0; i < px->nPixels; i++, level *= 0.8)
			{
				px->colors[px->nPixels + i - 1].HSV(px->H.getNewVal<float>(), px->S.getNewVal<float>(), level);
				px->colors[px->nPixels - i - 1] = px->colors[px->nPixels + i - 1];
			}
		}

		uint32_t update() override
		{
			px->pixel->set(px->colors + phase, px->nPixels);
			if (phase == px->nPixels - 1)
				dir = -1;
			else if (phase == 0)
				dir = 1;
			phase += dir;
			return (20);
		}

		int requiredBuffer() override
		{
			return (px->nPixels * 2 - 1);
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	struct Random3 : SpecialEffect
	{

		Random3(Pixel_Strand *px) : SpecialEffect{ px, "Random" } {}

		uint32_t update() override
		{
			for (int i = 0; i < px->nPixels; i++)
				px->colors[i].HSV((esp_random() % 6) * 60, 100, px->V.getNewVal<float>());
			px->pixel->set(px->colors, px->nPixels);
			return (1000);
		}

		int requiredBuffer() override
		{
			return (px->nPixels);
		}
	};


	///////////////////////////////

	struct Twinkle4 : SpecialEffect
	{

		int8_t *dir;

		Twinkle4(Pixel_Strand *px)
		  : SpecialEffect{ px, "Twinkle" }
		{
			dir = (int8_t *)calloc(px->nPixels, sizeof(int8_t));
		}

		void init() override
		{

			for (int i = 0; i < px->nPixels; i++)
			{
				px->colors[i].RGB(0, 0, 0, 0);
				dir[i] = 0;
			}
		}

		uint32_t update() override
		{
			for (int i = 0; i < px->nPixels; i++)
			{
				if (px->colors[i] == Pixel::Color().RGB(0, 0, 0, 0))
				{
					if (esp_random() % 200 == 0)
						dir[i] = 15;
					else
						dir[i] = 0;
				}
				else if (px->colors[i] == Pixel::Color().RGB(0, 0, 0, 255) || esp_random() % 10 == 0)
				{
					dir[i] = -15;
				}
				px->colors[i] += Pixel::Color().RGB(0, 0, 0, dir[i]);
			}
			px->pixel->set(px->colors, px->nPixels);
			return (50);
		}

		int requiredBuffer() override
		{
			return (px->nPixels);
		}
	};

	///////////////////////////////

	struct RaceTrack5 : SpecialEffect
	{

		int H = 0;
		int phase = 0;
		int dir = 1;

		RaceTrack5(Pixel_Strand *px)
		  : SpecialEffect{ px, "RaceTrack" } {}

		uint32_t update() override
		{
			for (int i = 0; i < px->nPixels; i++)
			{
				if (i == phase)
					px->colors[i].HSV(H, 100, px->V.getNewVal<float>());
				else if (i == px->nPixels - 1 - phase)
					px->colors[i].HSV(H + 180, 100, px->V.getNewVal<float>());
				else
					px->colors[i].RGB(0, 0, 0, 0);
			}

			px->pixel->set(px->colors, px->nPixels);
			phase = (phase + dir) % px->nPixels;

			if (phase == 0)
			{
				dir = 1;
				H = (H + 10) % 360;
			}
			else if (phase == px->nPixels - 1)
			{
				dir = -1;
				H = (H + 10) % 360;
			}

			return (20);
		}

		int requiredBuffer() override
		{
			return (px->nPixels);
		}
	};
};
