///////////////////////////////////////////////////////////////////////////////
// Display on the screen. Current status

#include <TFT_eSPI.h>       // Include the graphics library
#include <SPI.h>


class Display : public TFT_eSPI
{
public:
	int R, G, B;
	int Brightness = 0;
	boolean Wifi = false;
	int Tick = 0;
	///////////////////////////////////////////////////////////
	// Setup display
	void Setup()
	{
		//_tft = TFT_eSPI();  // Create object "tft"

		// Setup the display
		begin();
		setRotation(3);
		fillScreen(TFT_BLACK);            // Clear screen
		setCursor(0, 0, 2);
		
		Show("Display setup");
	}

	void Show(char* pText)
	{
		setTextSize(2);
		println(pText);
	}

	void Refresh()
	{
		fillScreen(TFT_BLACK);

		// Draw color
		fillRect(0, 0, 42, 42, TFT_WHITE);
		fillRect(1, 1, 40, 40, color565(R, G, B));

		// Setup for text
		setCursor(0, 0, 2);
		setTextColor(TFT_WHITE);
		setTextSize(2);

		// Draw brightness
		drawNumber(Brightness,0,40,2);

		// Draw wifi
		if( Wifi )
			drawString("W", 0, 65);
		else
			drawString("X", 0, 65);

			
		drawNumber( Tick++, 100, 10);
	}
};
