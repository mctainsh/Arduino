#ifndef DisplayI2C_h
#define DisplayI2C_h


#include <inttypes.h>
#if (defined(__AVR__) || defined(ARDUINO_ARCH_AVR))
# include <avr/pgmspace.h>
#endif
#include <Adafruit_NeoPixel.h>
#include "GraphicsLib.h"


class DisplayI2C : public GraphicsLib
{
public:
	DisplayI2C() {}
	void begin(int_least16_t width, int_least16_t height, uint_least16_t o, Adafruit_NeoPixel *strip )
	{
		lcd_width = width;
		lcd_height = height;
		lcd_orientation = o;
		_strip = strip;
		_strip->begin();
		_strip->show();
		_strip->setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)


		_strip->rainbow(127, 2, 255, 255, true);
		_strip->show();
		Serial.println("Rainbow");
	}

	void setOrientation(uint_least16_t o) {}
	void setArea(int_least16_t x0, int_least16_t y0, int_least16_t x1, int_least16_t y1)
	{
		_x0 = x0;
		_y0 = y0;
		_x1 = x1;
		_y1 = y1;
	}
	void drawStart()
	{
		//Serial.println("drawStart");
	}
	void draw(int32_t color)
	{
		//Serial.printf("%d,%d,%d,%d\r\n", _x0, _y0, _x1, _y1);
		
		// TODO : Apply range limits
		if( _x1 < 0 || _y1 < 0 || _x0 > lcd_width || _y0 > lcd_height ) 
			return;
		
		int16_t r;
		for( int16_t x = _x0; x <= _x1; x++ )
			for( int16_t y = _y0; y <= _y1; y++ )
			{
				// Short wide
				if( x % 2 )
					r = (x+1) * lcd_height - y - 1;
				else
					r = (x * lcd_height + y);
				if( r >= 0 )
					_strip->setPixelColor(r, color);
				//Serial.printf("Set p %d,%d\r\n", x, y);
			}
	}
	void drawStop()
	{
		_strip->show();
		//Serial.println("show()");
	}

private:
	Adafruit_NeoPixel* _strip;
	uint_least16_t _x0, _x1, _y0, _y1;
};


#endif //DisplayI2C_h
