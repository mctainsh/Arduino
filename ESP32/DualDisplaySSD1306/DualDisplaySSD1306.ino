/**************************************************************************
    This is an example for our Monochrome OLEDs based on SSD1306 drivers

    Pick one up today in the adafruit shop!
    ------> http://www.adafruit.com/category/63_98

    This example is for a 128x64 pixel display using I2C to communicate
    3 pins are required to interface (two I2C and one reset).

    Adafruit invests time and resources providing this open
    source code, please support Adafruit and open-source
    hardware by purchasing products from Adafruit!

    Written by Limor Fried/Ladyada for Adafruit Industries,
    with contributions from the open source community.
    BSD license, check license.txt for more information
    All text above, and the splash screen below must be
    included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

//Constants
#define DHTPIN 17     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

#define WIDTH ((int16_t)128) // OLED display width, in pixels
#define HEIGHT ((int16_t)64) // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display1(WIDTH, HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(WIDTH, HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{
	B00000000, B11000000,
	B00000001, B11000000,
	B00000001, B11000000,
	B00000011, B11100000,
	B11110011, B11100000,
	B11111110, B11111000,
	B01111110, B11111111,
	B00110011, B10011111,
	B00011111, B11111100,
	B00001101, B01110000,
	B00011011, B10100000,
	B00111111, B11100000,
	B00111111, B11110000,
	B01111100, B11110000,
	B01110000, B01110000,
	B00000000, B00110000
};

void setup()
{
	Serial.begin(115200);
	ScanI2C();
	Serial.println("Starting");

	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		// Address 0x3D for 128x64
		Serial.println(F("SSD1306 allocation 0x3C failed"));
		for (;;); // Don't proceed, loop forever
	}
	if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3D))
	{
		// Address 0x3D for 128x64
		Serial.println(F("SSD1306 allocation 0x3D  failed"));
		for (;;); // Don't proceed, loop forever
	}
	Serial.println("Setting up DHT22 on pin " + DHTPIN);
	dht.begin();

	// Show initial display buffer contents on the screen --
	// the library initializes this with an Adafruit splash screen.
	display1.display();
	display2.display();
	delay(2000); // Pause for 2 seconds
}

void loop()
{
	// Clear the buffer
	display1.clearDisplay();
	display2.clearDisplay();

	// Draw a single pixel in white
	display1.drawPixel(10, 10, SSD1306_WHITE);

	// Show the display buffer on the screen. You MUST call display() after
	// drawing commands to make them visible on screen!
	display1.display();
	delay(2000);
	// display1.display() is NOT necessary after every single drawing command,
	// unless that's what you want...rather, you can batch up a bunch of
	// drawing operations and then update the screen all at once by calling
	// display1.display(). These examples demonstrate both approaches...

	displayTempAndHumidity();
	testdrawline();      // Draw many lines
	displayTempAndHumidity();
	testdrawrect();      // Draw rectangles (outlines)
	displayTempAndHumidity();
	testfillrect();      // Draw rectangles (filled)
	displayTempAndHumidity();
	testdrawcircle();    // Draw circles (outlines)
	displayTempAndHumidity();
	testfillcircle();    // Draw circles (filled)
	displayTempAndHumidity();
	testdrawroundrect(); // Draw rounded rectangles (outlines)
	displayTempAndHumidity();
	testfillroundrect(); // Draw rounded rectangles (filled)
	displayTempAndHumidity();

	testdrawtriangle();  // Draw triangles (outlines)
	displayTempAndHumidity();

	testfilltriangle();  // Draw triangles (filled)
	displayTempAndHumidity();

	testdrawchar();      // Draw characters of the default font
	displayTempAndHumidity();

	testdrawstyles();    // Draw 'stylized' characters
	displayTempAndHumidity();

	testscrolltext();    // Draw scrolling text
	displayTempAndHumidity();

	testdrawbitmap();    // Draw a small bitmap image

	// Invert and restore display, pausing in-between
	display1.invertDisplay(true);
	delay(1000);
	display1.invertDisplay(false);
	delay(1000);

	testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}

void displayTempAndHumidity()
{
	//Read data and store it to variables hum and temp
	hum = dht.readHumidity();
	temp = dht.readTemperature();
	//Print temp and humidity values to serial monitor
	Serial.print("Humidity: ");
	Serial.print(hum);
	Serial.print(" %, Temp: ");
	Serial.print(temp);
	Serial.println(" Celsius");



	display1.clearDisplay();
	display2.clearDisplay();

	display1.setTextSize(2); // Draw 2X-scale text
	display1.setTextColor(SSD1306_WHITE);
	display2.setTextSize(2); // Draw 2X-scale text
	display2.setTextColor(SSD1306_WHITE);
	display1.setCursor(10, 0);
	display2.setCursor(10, 0);

	display1.println(F("Temp"));
	display1.println(temp);

	display2.println(F("Humidity"));
	display2.println(hum);


	display1.display();      // Show initial text
	display2.display();      // Show initial text




	delay(3000); //Delay 2 sec.
}

void testdrawline()
{
	int16_t i;

	display2.clearDisplay(); // Clear display buffer

	for (i = 0; i < WIDTH; i += 4)
	{
		display2.drawLine(0, 0, i, HEIGHT - 1, SSD1306_WHITE);
		display2.display(); // Update screen with each newly-drawn line
		delay(1);
	}
	for (i = 0; i < HEIGHT; i += 4)
	{
		display2.drawLine(0, 0, WIDTH - 1, i, SSD1306_WHITE);
		display2.display();
		delay(1);
	}
	delay(250);

	display2.clearDisplay();

	for (i = 0; i < WIDTH; i += 4)
	{
		display2.drawLine(0, HEIGHT - 1, i, 0, SSD1306_WHITE);
		display2.display();
		delay(1);
	}
	for (i = HEIGHT - 1; i >= 0; i -= 4)
	{
		display2.drawLine(0, HEIGHT - 1, WIDTH - 1, i, SSD1306_WHITE);
		display2.display();
		delay(1);
	}
	delay(250);

	display2.clearDisplay();

	for (i = WIDTH - 1; i >= 0; i -= 4)
	{
		display2.drawLine(WIDTH - 1, HEIGHT - 1, i, 0, SSD1306_WHITE);
		display2.display();
		delay(1);
	}
	for (i = HEIGHT - 1; i >= 0; i -= 4)
	{
		display2.drawLine(WIDTH - 1, HEIGHT - 1, 0, i, SSD1306_WHITE);
		display2.display();
		delay(1);
	}
	delay(250);

	display2.clearDisplay();

	for (i = 0; i < HEIGHT; i += 4)
	{
		display1.drawLine(WIDTH - 1, 0, 0, i, SSD1306_WHITE);
		display1.display();
		delay(1);
	}
	for (i = 0; i < WIDTH; i += 4)
	{
		display1.drawLine(WIDTH - 1, 0, i, HEIGHT - 1, SSD1306_WHITE);
		display1.display();
		delay(1);
	}

	delay(2000); // Pause for 2 seconds
}

void testdrawrect(void)
{
	display1.clearDisplay();

	for (int16_t i = 0; i < HEIGHT / 2; i += 2)
	{
		display1.drawRect(i, i, WIDTH - 2 * i, HEIGHT - 2 * i, SSD1306_WHITE);
		display1.display(); // Update screen with each newly-drawn rectangle
		delay(1);
	}

	delay(2000);
}

void testfillrect(void)
{
	display2.clearDisplay();

	for (int16_t i = 0; i < HEIGHT / 2; i += 3)
	{
		// The INVERSE color is used so rectangles alternate white/black
		display2.fillRect(i, i, WIDTH - i * 2, HEIGHT - i * 2, SSD1306_INVERSE);
		display2.display(); // Update screen with each newly-drawn rectangle
		delay(1);
	}

	delay(2000);
}

void testdrawcircle(void)
{
	display1.clearDisplay();

	for (int16_t i = 0; i < max(WIDTH, HEIGHT) / 2; i += 2)
	{
		display1.drawCircle(WIDTH / 2, HEIGHT / 2, i, SSD1306_WHITE);
		display1.display();
		delay(1);
	}

	delay(2000);
}

void testfillcircle(void)
{
	display2.clearDisplay();

	for (int16_t i = max(WIDTH, HEIGHT) / 2; i > 0; i -= 3)
	{
		// The INVERSE color is used so circles alternate white/black
		display2.fillCircle(WIDTH / 2, HEIGHT / 2, i, SSD1306_INVERSE);
		display2.display(); // Update screen with each newly-drawn circle
		delay(1);
	}

	delay(2000);
}

void testdrawroundrect(void)
{
	display1.clearDisplay();

	for (int16_t i = 0; i < HEIGHT / 2 - 2; i += 2)
	{
		display1.drawRoundRect(i, i, WIDTH - 2 * i, HEIGHT - 2 * i,
		                       HEIGHT / 4, SSD1306_WHITE);
		display1.display();
		delay(1);
	}

	delay(2000);
}

void testfillroundrect(void)
{
	display2.clearDisplay();

	for (int16_t i = 0; i < HEIGHT / 2 - 2; i += 2)
	{
		// The INVERSE color is used so round-rects alternate white/black
		display2.fillRoundRect(i, i, WIDTH - 2 * i, HEIGHT - 2 * i,
		                       HEIGHT / 4, SSD1306_INVERSE);
		display2.display();
		delay(1);
	}

	delay(2000);
}

void testdrawtriangle(void)
{
	display1.clearDisplay();

	for (int16_t i = 0; i < max(WIDTH, HEIGHT) / 2; i += 5)
	{
		display1.drawTriangle(
		    WIDTH / 2  , HEIGHT / 2 - i,
		    WIDTH / 2 - i, HEIGHT / 2 + i,
		    WIDTH / 2 + i, HEIGHT / 2 + i, SSD1306_WHITE);
		display1.display();
		delay(1);
	}

	delay(2000);
}

void testfilltriangle(void)
{
	display2.clearDisplay();

	for (int16_t i = max(WIDTH, HEIGHT) / 2; i > 0; i -= 5)
	{
		// The INVERSE color is used so triangles alternate white/black
		display2.fillTriangle(
		    WIDTH / 2  , HEIGHT / 2 - i,
		    WIDTH / 2 - i, HEIGHT / 2 + i,
		    WIDTH / 2 + i, HEIGHT / 2 + i, SSD1306_INVERSE);
		display2.display();
		delay(1);
	}

	delay(2000);
}

void testdrawchar(void)
{
	display1.clearDisplay();

	display1.setTextSize(1);      // Normal 1:1 pixel scale
	display1.setTextColor(SSD1306_WHITE); // Draw white text
	display1.setCursor(0, 0);     // Start at top-left corner
	display1.cp437(true);         // Use full 256 char 'Code Page 437' font

	// Not all the characters will fit on the display1. This is normal.
	// Library will draw what it can and the rest will be clipped.
	for (int16_t i = 0; i < 256; i++)
	{
		if (i == '\n') display1.write(' ');
		else          display1.write(i);
	}

	display1.display();
	delay(2000);
}

void testdrawstyles(void)
{
	display2.clearDisplay();

	display2.setTextSize(1);             // Normal 1:1 pixel scale
	display2.setTextColor(SSD1306_WHITE);        // Draw white text
	display2.setCursor(0, 0);            // Start at top-left corner
	display2.println(F("Hello, world!"));

	display2.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
	display2.println(3.141592);

	display2.setTextSize(2);             // Draw 2X-scale text
	display2.setTextColor(SSD1306_WHITE);
	display2.print(F("0x"));
	display2.println(0xDEADBEEF, HEX);

	display2.display();
	delay(2000);
}

void testscrolltext(void)
{
	display1.clearDisplay();

	display1.setTextSize(2); // Draw 2X-scale text
	display1.setTextColor(SSD1306_WHITE);
	display1.setCursor(10, 0);
	display1.println(F("scroll"));
	display1.display();      // Show initial text
	delay(100);

	// Scroll in various directions, pausing in-between:
	display1.startscrollright(0x00, 0x0F);
	delay(2000);
	display1.stopscroll();
	delay(1000);
	display1.startscrollleft(0x00, 0x0F);
	delay(2000);
	display1.stopscroll();
	delay(1000);
	display1.startscrolldiagright(0x00, 0x07);
	delay(2000);
	display1.startscrolldiagleft(0x00, 0x07);
	delay(2000);
	display1.stopscroll();
	delay(1000);
}

void testdrawbitmap(void)
{
	display2.clearDisplay();

	display2.drawBitmap(
	    (WIDTH  - LOGO_WIDTH ) / 2,
	    (HEIGHT - LOGO_HEIGHT) / 2,
	    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
	display2.display();
	delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2
int tempLoop = 0;

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h)
{
	int8_t f, icons[NUMFLAKES][3];

	// Initialize 'snowflake' positions
	for (f = 0; f < NUMFLAKES; f++)
	{
		icons[f][XPOS]   = random(1 - LOGO_WIDTH, WIDTH);
		icons[f][YPOS]   = -LOGO_HEIGHT;
		icons[f][DELTAY] = random(1, 6);
		Serial.print(F("x: "));
		Serial.print(icons[f][XPOS], DEC);
		Serial.print(F(" y: "));
		Serial.print(icons[f][YPOS], DEC);
		Serial.print(F(" dy: "));
		Serial.println(icons[f][DELTAY], DEC);
	}

	for (;;)
	{
		// Loop forever...
		display1.clearDisplay(); // Clear the display buffer
		display2.clearDisplay(); // Clear the display buffer

		// Draw each snowflake:
		for (f = 0; f < NUMFLAKES; f++)
		{
			display1.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
			display2.drawBitmap(icons[f][YPOS], icons[f][XPOS], bitmap, w, h, SSD1306_WHITE);
		}

		display1.display(); // Show the display buffer on the screen
		display2.display(); // Show the display buffer on the screen
		delay(200);        // Pause for 1/10 second

		// Then update coordinates of each flake...
		for (f = 0; f < NUMFLAKES; f++)
		{
			icons[f][YPOS] += icons[f][DELTAY];
			// If snowflake is off the bottom of the screen...
			if (icons[f][YPOS] >= HEIGHT)
			{
				// Reinitialize to a random position, just off the top
				icons[f][XPOS]   = random(1 - LOGO_WIDTH, WIDTH);
				icons[f][YPOS]   = -LOGO_HEIGHT;
				icons[f][DELTAY] = random(1, 6);
			}
		}

		// At roll over displat temp and humidity
		if( tempLoop++ > 100 )
		{
			tempLoop = 0;
			return;
		}
	}
}



void ScanI2C()
{
	byte error, address;
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;
	for (address = 1; address < 127; address++ )
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println("  !");

			nDevices++;
		}
		else if (error == 4)
		{
			Serial.print("Unknown error at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial.println("No I2C devices found\n");
	else
		Serial.println("done\n");

	delay(5000);           // wait 5 seconds for next scan
}
