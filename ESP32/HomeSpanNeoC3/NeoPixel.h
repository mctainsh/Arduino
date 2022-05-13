// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    2		// WAS 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 108		// Home Bed
//#define LED_COUNT 300		// Long strip
class NeoPixel : public Adafruit_NeoPixel
{
	public :
		enum BarMode { off, plain, rainbowBar, dawn };

	private:
		int _wipePosition = 0;
		int32_t _currentColor;
		unsigned long _lockOutTime;			// Time then no animation should occur
		BarMode _mode = BarMode::off;

public:

// Declare our NeoPixel strip object:
	NeoPixel() : Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800)
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
	{

	}


	void Setup()
	{
		// These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
		// Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
		clock_prescale_set(clock_div_1);
#endif
		// END of Trinket-specific code.

		begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
		show();            // Turn OFF all pixels ASAP
		setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
	}

	void Off()
	{
		setBrightness(0);
		clear();
		show();
		SetMode(BarMode::off);
	}

	void SetMode(BarMode mode)
	{
		_mode = mode;
	}

	void Loop()
	{
		switch( _mode )
		{
			case rainbowBar : RainbowLoop(); break;
		}
	}

	void Fill( int r, int g, int b, int count )
	{
		_lockOutTime = millis();
		_currentColor = Color( r, g, b);
		fill(_currentColor, 0, count);
		show();
	}
	void FillHSV( int h, int s, int v )
	{
		_lockOutTime = millis();
		_currentColor = gamma32(ColorHSV( h, s, v));
		fill(_currentColor, 0, LED_COUNT);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Move the whipe action along one step
	void Wipe()
	{
		// Are we locked out
		unsigned long m = millis();
		if( _lockOutTime < m && m < _lockOutTime + 2000 )
			return;

		// Fix overflow
		_wipePosition++;
		if( _wipePosition > LED_COUNT )
			_wipePosition = 0;

		// Draw in the wipe bar
		clear();
		for( int n = _wipePosition; n < _wipePosition + 20; n++)
			setPixelColor(n < LED_COUNT ? n : n-LED_COUNT, _currentColor);
		show();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Make a sign wave of the intensity
	void Wave()
	{
		// Are we locked out
		unsigned long m = millis();
		if( _lockOutTime < m && m < _lockOutTime + 2000 )
			return;

		// Fix overflow
		_wipePosition++;
		if( _wipePosition > 360 )
			_wipePosition = 0;

		// Draw in the wave bar

		float r = (uint8_t)(_currentColor >> 16);
		float g = (uint8_t)(_currentColor >> 8);
		float b = (uint8_t)(_currentColor);

		clear();
		int32_t c;
		float f;
		for( int n = 0; n < LED_COUNT; n++)
		{
			f = (sin((float)(n+_wipePosition)/180*PI) + 2.0 )/ 3.0;
			//if( f < 0 )
			//	f*=-1;
			//if( f > 1 )
			//	f-=1;
			c = Color( f*r, f*g, f*b);
			setPixelColor(n, c);


			//Serial.printf("F=%f, R=%f, G=%f, B=%f, fr=%d, fg=%d, fb=%d\r\n", f, r, g, b,f*r, f*g, f*b );


		}
		show();
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Rainbow cycle along whole strip. from 0 to 65536
	void RainbowLoop()
	{
		// Increment and fix colour alignment
		_currentColor+= 5;
		if( _currentColor % 5 )
			_currentColor = 0;

		// Only update every 50
		if( _currentColor % 250)
			return;

		// Wrap colours
		if( 0 > _currentColor || _currentColor > 65536 )
			_currentColor = 0;

		// Apply
		rainbow(_currentColor);
		show();
	}
};
