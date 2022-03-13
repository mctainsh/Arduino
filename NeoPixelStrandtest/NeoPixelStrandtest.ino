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

// Thearter lights gap
#define TGAP 10

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 300

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


#include <TFT_eSPI.h>       // Include the graphics library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Create object "tft"

// setup() function -- runs once at startup --------------------------------

void setup()
{
	// Setup the display
	tft.begin();
	tft.setRotation(1);

	// Set text datum to middle centre
	//tft.setTextDatum(MC_DATUM);

	// Set text colour to orange with black background
	//tft.setTextColor(TFT_WHITE, TFT_BLACK);

	// These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
	// Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
	clock_prescale_set(clock_div_1);
#endif
	// END of Trinket-specific code.

	strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
	strip.show();            // Turn OFF all pixels ASAP
	strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

	Serial.begin(9600);
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop()
{
	// Fill along the length of the strip in various colors...
	colorWipe(255,   0,   0, 1); // Red
	colorWipe(  0, 255,   0, 1); // Green
	colorWipe(  0,   0, 255, 1); // Blue
	rainbow(0,1024);             // Flowing rainbow cycle along the whole strip

	// Do a theater marquee effect in various colors...
	//theaterChase(strip.Color(127, 127, 127), 10); // White, half brightness
	//theaterChase(strip.Color(127,   0,   0), 10); // Red, half brightness
	//theaterChase(strip.Color(  0,   0, 127), 10); // Blue, half brightness

//	rainbow(0,1024);             // Flowing rainbow cycle along the whole strip
	theaterChaseRainbow(10); // Rainbow-enhanced theaterChase variant
}

void Display(char* pText)
{
	tft.fillScreen(0x5AEB);
	//tft.fillScreen(TFT_BLACK);            // Clear screen
	//tft.setFreeFont(FF18);                 // Select the font
	//tft.drawString(text, 160, 60, GFXFF);// Print the string name of the font

	tft.setCursor(0, 0, 2);
	tft.setTextColor(TFT_WHITE,TFT_BLACK);
	tft.setTextSize(3);
	tft.println(pText);
}


// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(int r, int g, int b, int wait)
{
	Display("Wipe");
	for(int i=0; i<strip.numPixels(); i++)  // For each pixel in strip...
	{
		//	for( double n = 0; n < MAX; n++ )
		//	{
		//		double factor = (MAX-n)/MAX;
		//		//Serial.println(factor);
		//		strip.setPixelColor(i-n, strip.Color( (int)(r*factor), (int)(g*factor), (int)(b*factor)));
		//	}
		colorWipeSetPixelState( r, g, b, i, false);
		colorWipeSetPixelState( r, g, b, i, true);
		strip.show();                          //  Update strip to match
		delay(wait);                           //  Pause for a moment
	}

//  	for(int i=strip.numPixels(); i>0; i--)  // For each pixel in strip...
// 	{
	//	for( double n = 0; n < MAX && (i-n)>0; n++ )
	//	{
	//		double factor = (MAX-n)/MAX;
	//Serial.println(factor);
	//		strip.setPixelColor(i+n, strip.Color( (int)(r*factor), (int)(g*factor), (int)(b*factor)));
	//	}
	//	strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
	//	strip.show();                          //  Update strip to match
	//	delay(wait);                           //  Pause for a moment
	//}
}

// Set the pixels for a wipe at a given pixel
void colorWipeSetPixelState( int r, int g, int b, int i, bool direction )
{
	if( !direction )
	{
		i = strip.numPixels() - i;
		int t = r;
		r = b;
		b = t;
	}
	const double MAX = 50;
	int32_t color = strip.Color( r, g, b);
	uint16_t x;
	uint32_t c0;
	for( double n = 0; n < MAX && (i-n)>0; n++ )
	{
		double factor = (MAX-n)/MAX;
		x = direction ? i-n : i+n;
		// Add colors together
		//c0 = strip.getPixelColor(x);
		c0 = strip.Color( (int)(r*factor), (int)(g*factor), (int)(b*factor));
		strip.setPixelColor(x, c0);
	}
	strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
}


// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait)
{
	for(int a=0; a<10; a++)   // Repeat 10 times...
	{
		for(int b=0; b<TGAP; b++)  //  'b' counts from 0 to 2...
		{
			strip.clear();         //   Set all pixels in RAM to 0 (off)
			// 'c' counts up from 'b' to end of strip in steps of 3...
			for(int c=b; c<strip.numPixels(); c += TGAP)
			{
				strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
			}
			strip.show(); // Update strip with new contents
			delay(wait);  // Pause for a moment
		}
	}
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait, int hueStep)
{
	Display("Rainbow");

	// Hue of first pixel runs 5 complete loops through the color wheel.
	// Color wheel has a range of 65536 but it's OK if we roll over, so
	// just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
	// means we'll make 5*65536/256 = 1280 passes through this loop:
	for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += hueStep)
	{
		// strip.rainbow() can take a single argument (first pixel hue) or
		// optionally a few extras: number of rainbow repetitions (default 1),
		// saturation and value (brightness) (both 0-255, similar to the
		// ColorHSV() function, default 255), and a true/false flag for whether
		// to apply gamma correction to provide 'truer' colors (default true).
		strip.rainbow(firstPixelHue);
		// Above line is equivalent to:
		// strip.rainbow(firstPixelHue, 1, 255, 255, true);
		strip.show(); // Update strip with new contents
		if( wait != 0 )
			delay(wait);  // Pause for a moment
	}
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait)
{
	Display("Theater");
	int firstPixelHue = 0;     // First pixel starts at red (hue 0)
	for(int a=0; a<30; a++)   // Repeat 30 times...
	{
		for(int b=0; b<TGAP; b++)  //  'b' counts from 0 to 2...
		{
			strip.clear();         //   Set all pixels in RAM to 0 (off)
			// 'c' counts up from 'b' to end of strip in increments of 3...
			for(int c=b; c<strip.numPixels(); c += TGAP)
			{
				// hue of pixel 'c' is offset by an amount to make one full
				// revolution of the color wheel (range 65536) along the length
				// of the strip (strip.numPixels() steps):
				int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
				uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
				strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
			}
			strip.show();                // Update strip with new contents
			delay(wait);                 // Pause for a moment
			firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
		}
	}
}
