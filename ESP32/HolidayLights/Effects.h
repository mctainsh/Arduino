#pragma once

int _effectIndex = 0;

///////////////////////////////////////////////////////////////////////////////////
// Effect base structure
struct SpecialEffect
{
	//Pixel_Strand *px;
	const char *name;
	float _h;
	float _s;
	float _v;

	SpecialEffect(const char *name)
	{
		//this->px = px;
		this->name = name;
		Serial.printf("Adding Effect %d: %s\n", _effectIndex++, name);
	}

	virtual void init(float h, float s, float v)
	{
		_h = h;
		_s = s;

		// Power 0 100 % can be 0 to 255
		_v = v * 0.50;
	}
	virtual uint32_t update()
	{
		return (10 * 1000);
	}
	virtual int requiredBuffer()
	{
		return (0);
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////
struct ManualControl_1 : SpecialEffect
{
	ManualControl_1()
	  : SpecialEffect{ "Manual Control" } {}

	void init(float h, float s, float v) override
	{
		SpecialEffect::init(h, s, v);
	}

	uint32_t update() override
	{
		TurnOnStrip(true);
		g_pixel.set(Pixel::Color().HSV(_h, _s, _v), PIXEL_COUNT);
		return (5 * 1000);
	}
};



/////////////////////////////////////////////////////////////////////////////////////////////
struct KnightRider_2 : SpecialEffect
{
	int phase = 0;
	int dir = 1;

	KnightRider_2()
	  : SpecialEffect{ "KnightRider" } {}

	void init(float h, float s, float v) override
	{
		SpecialEffect::init(h, s, v);
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

	Random_3()
	  : SpecialEffect{ "Random" } {}

	void init(float h, float s, float v) override
	{
		SpecialEffect::init(h, s, v);
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
	Twinkle_4()
	  : SpecialEffect{ "Twinkle" }
	{
		_dir = (int8_t *)calloc(PIXEL_COUNT, sizeof(int8_t));
		_colours = (int8_t *)calloc(PIXEL_COUNT, sizeof(int8_t));
	}

	// Clear all the pixels
	void init(float h, float s, float v) override
	{
		SpecialEffect::init(h, s, v);

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

	Rainbow_5()
	  : SpecialEffect{ "Rainbow" } {}

	void init(float h, float s, float v) override
	{
		SpecialEffect::init(h, s, v);
	}

	uint32_t update() override
	{
		for (int i = 0; i < PIXEL_COUNT; i++)
			g_colors[i].HSV(_startHue + i, 100, _v);
		_startHue += 1;	 // Slow progressions
		//_startHue += (_s / 20 );		// Progression based on saturation
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

	RaceTrack_6()
	  : SpecialEffect("RaceTrack")
	{
	}

	void init(float h, float s, float v) override
	{
		SpecialEffect::init(h, s, v);
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
