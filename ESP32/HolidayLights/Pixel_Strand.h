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

	///////////////////////////////////////////////////////////////////////////////////
	// Effect base structure
	struct SpecialEffect
	{
		//Pixel_Strand *px;
		const char *name;
		float _h;
		float _s;
		float _v;

		SpecialEffect(Pixel_Strand *px, const char *name)
		{
			//this->px = px;
			this->name = name;
			Serial.printf("Adding Effect %d: %s\n", px->Effects.size() + 1, name);
		}

		virtual void init(float h, float s, float v) {}
		virtual uint32_t update()
		{
			return (10 * 1000);
		}
		virtual int requiredBuffer()
		{
			return (0);
		}
	};

	// Collection of effects
	vector<SpecialEffect *> Effects;

	// Time before the alarm activates again
	uint32_t _alarmTime;

	Pixel_Strand(Pixel *pixelIn, int nPixels)
	  : Service::LightBulb()
	{
		Effects.push_back(new ManualControl_1(this));
		Effects.push_back(new KnightRider_2(this));
		Effects.push_back(new Random_3(this));
		Effects.push_back(new Twinkle_4(this));
		Effects.push_back(new Rainbow_5(this));
		Effects.push_back(new RaceTrack_6(this));

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

	/////////////////////////////////////////////////////////////////////////////////////////////
	struct ManualControl_1 : SpecialEffect
	{
		ManualControl_1(Pixel_Strand *px)
		  : SpecialEffect{ px, "Manual Control" } {}

		void init(float h, float s, float v) override
		{
			_h = h;
			_s = s;
			_v = v;
			TurnOnStrip(true);
			g_pixel.set(Pixel::Color().HSV(h, s, v), PIXEL_COUNT);
		}

		// TODO : Refresh every 5 seconds
	};


	/////////////////////////////////////////////////////////////////////////////////////////////
	struct KnightRider_2 : SpecialEffect
	{
		int phase = 0;
		int dir = 1;

		KnightRider_2(Pixel_Strand *px)
		  : SpecialEffect{ px, "KnightRider" } {}

		void init(float h, float s, float v) override
		{
			_h = h;
			_s = s;
			_v = v;
			float level = _v;
			for (int i = 0; i < PIXEL_COUNT; i++, level *= 0.8)
			{
				g_colors[PIXEL_COUNT + i - 1].HSV(_h, _s, level);
				g_colors[PIXEL_COUNT - i - 1] = g_colors[PIXEL_COUNT + i - 1];
			}
		}

		uint32_t update() override
		{
			g_pixel.set(g_colors + phase, PIXEL_COUNT);
			if (phase == PIXEL_COUNT - 1)
				dir = -1;
			else if (phase == 0)
				dir = 1;
			phase += dir;
			return (20);
		}

		int requiredBuffer() override
		{
			return (PIXEL_COUNT * 2 - 1);
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	struct Random_3 : SpecialEffect
	{

		Random_3(Pixel_Strand *px)
		  : SpecialEffect{ px, "Random" } {}

		void init(float h, float s, float v) override
		{
			_h = h;
			_s = s;
			_v = v;
		}

		uint32_t update() override
		{
			for (int i = 0; i < PIXEL_COUNT; i++)
				g_colors[i].HSV((esp_random() % 6) * 60, 100, _v);
			g_pixel.set(g_colors, PIXEL_COUNT);
			return (1000);
		}

		int requiredBuffer() override
		{
			return (PIXEL_COUNT);
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////
	struct Twinkle_4 : SpecialEffect
	{

		int8_t *_dir;	   // Array of directions to apply to the data
		int8_t *_colours;  // Colour to apply 0 = R, 1 = G,

		// Allocate an array of direction values
		Twinkle_4(Pixel_Strand *px)
		  : SpecialEffect{ px, "Twinkle" }
		{
			_dir = (int8_t *)calloc(PIXEL_COUNT, sizeof(int8_t));
			_colours = (int8_t *)calloc(PIXEL_COUNT, sizeof(int8_t));
		}

		// Clear all the pixels
		void init(float h, float s, float v) override
		{
			_h = h;
			_s = s;
			_v = v;
			for (int i = 0; i < PIXEL_COUNT; i++)
			{
				g_colors[i].RGB(0, 0, 0);
				_dir[i] = 0;
			}
		}

		uint32_t update() override
		{
			const Pixel::Color BLACK = Pixel::Color().RGB(0, 0, 0);
			for (int i = 0; i < PIXEL_COUNT; i++)
			{
				if (g_colors[i] == BLACK)
				{
					// (0.5%) of the time Start turning on the star
					if (esp_random() % 200 == 0)
					{
						_dir[i] = 15;
						_colours[i] = (esp_random() % 8) + 1;
					}
					else
					{
						_dir[i] = 0;
					}
				}
				//else if (g_colors[i] == BLACK || esp_random() % 10 == 0)
				else if (esp_random() % 10 == 0)
				{
					// Randomly dim it 10%
					_dir[i] = -15;
				}

				int8_t dir = _dir[i];
				int8_t color = _colours[i];
				// Add the direction value to the pixel
				g_colors[i] += Pixel::Color().RGB(
				  (color & 0x01) > 0 ? dir : 0,
				  (color & 0x02) > 0 ? dir : 0,
				  (color & 0x04) > 0 ? dir : 0);
			}
			g_pixel.set(g_colors, PIXEL_COUNT);
			return (50);
		}

		int requiredBuffer() override
		{
			return (PIXEL_COUNT);
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	struct Rainbow_5 : SpecialEffect
	{
		int _startHue = 0;

		Rainbow_5(Pixel_Strand *px)
		  : SpecialEffect{ px, "Rainbow" } {}

		void init(float h, float s, float v) override
		{
			_h = h;
			_s = s;
			_v = v;
		}

		uint32_t update() override
		{
			for (int i = 0; i < PIXEL_COUNT; i++)
				g_colors[i].HSV(_startHue + i, 100, _v);
			_startHue += (_s / 20 );
			g_pixel.set(g_colors, PIXEL_COUNT);
			return (1);
		}

		int requiredBuffer() override
		{
			return (PIXEL_COUNT);
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	struct RaceTrack_6 : SpecialEffect
	{

		int H = 0;
		int phase = 0;
		int dir = 1;

		RaceTrack_6(Pixel_Strand *px)
		  : SpecialEffect{ px, "RaceTrack" } {}

		void init(float h, float s, float v) override
		{
			_h = h;
			_s = s;
			_v = v;
		}

		uint32_t update() override
		{
			for (int i = 0; i < PIXEL_COUNT; i++)
			{
				if (i == phase)
					g_colors[i].HSV(H, 100, _v);
				else if (i == PIXEL_COUNT - 1 - phase)
					g_colors[i].HSV(H + 180, 100, _v);
				else
					g_colors[i].RGB(0, 0, 0, 0);
			}

			g_pixel.set(g_colors, PIXEL_COUNT);
			phase = (phase + dir) % PIXEL_COUNT;

			if (phase == 0)
			{
				dir = 1;
				H = (H + 10) % 360;
			}
			else if (phase == PIXEL_COUNT - 1)
			{
				dir = -1;
				H = (H + 10) % 360;
			}
			return (20);
		}

		int requiredBuffer() override
		{
			return (PIXEL_COUNT);
		}
	};
};
