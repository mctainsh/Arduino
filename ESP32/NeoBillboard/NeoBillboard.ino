/*
  GraphicsLib Demo

  This sketch demonstrates the functions of the GraphicsLib.
 */
#include <Adafruit_NeoPixel.h>

#include "GraphicsLib.h"
#include "NeoPixelGrid.h"
#include "Font_6x8_MSB.h"


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    2

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


DisplayI2C lcd; //I2C (GLCD-Shield or MI0283QT Adapter v2)
char text[256];


void setup()
{
	for( byte i = 1; i < 255; i++ )
		text[i-1] = i;
	text[255] = '\0';

	Serial.begin(115200);
	uint8_t clear_bg=0x00; //0x80 = dont clear background for fonts (only for DisplayXXX)

	//init display
 	lcd.begin( 32, 8, 0, &strip);

	//clear screen
	lcd.fillScreen(RGB(0, 255,0));
	delay(1000);

	//clear screen
	lcd.fillScreen(RGB(0,0,255));
	lcd.drawLine(0,0, 31, 7, RGB(255,0,0));
	delay(1000);

	//setOrientation
//	lcd.setOrientation(0);
	lcd.fillScreen(RGB(0,0,0));
	lcd.drawText(0, 0, "\xDB\xDBWg2\xB2", RGB(255,255,255), RGB(0,0,0));
//	lcd.setOrientation(90);
//	lcd.drawText(1, 1, "test 2", RGB(0,0,0), RGB(255,255,255), 1);
//	lcd.setOrientation(180);
//	lcd.drawText(1, 1, "test 3", RGB(0,0,0), RGB(255,255,255), 1);
//	lcd.setOrientation(270);
//	lcd.drawText(1, 1, "test 4", RGB(0,0,0), RGB(255,255,255), 1);
//	lcd.setOrientation(0);
	lcd.drawStop();
	delay(1000);
	return;

	//drawPixel, drawLine, drawRect, fillRect, drawTriangle, fillTriangle, drawRoundRect, fillRoundRect, drawCircle, fillCircle, drawEllipse, fillEllipse
	lcd.fillEllipse(0,7,0,30,RGB(200,  0,  0));
	lcd.drawEllipse(0,7,0,30,RGB(  0,  0,  0));
	delay(2000);

/*	lcd.fillCircle(50,30,10,RGB(  0,200,  0));
	lcd.drawCircle(50,30,12,RGB(  0,  0,  0));

	lcd.fillRoundRect(71,21,18,18,5,RGB(  0,  0,200));
	lcd.drawRoundRect(70,20,20,20,5,RGB(  0,  0,  0));

	lcd.fillTriangle(30,50,10,90,50,80,RGB(  0,200,200));
	lcd.drawTriangle(30,50,10,90,50,80,RGB(  0,  0,  0));

	lcd.fillRect(50,50,20,20,RGB(200,200,  0));
	lcd.drawRect(51,51,18,18,RGB(  0,  0,  0));

	lcd.drawLine(10,10,50,60,RGB( 10, 10, 10));
	lcd.drawLine(10,60,50,10,RGB( 10, 10, 10));

	lcd.drawPixel(5,5,RGB(100,100,100));


	//drawText
	lcd.drawText(50, 20, "String from RAM", RGB(0,0,0), RGB(255,255,255), 1|clear_bg);            //string from ram

	//drawInteger
	lcd.drawInteger(5, 20, 1234, DEC, RGB(0,0,0), RGB(255,255,255), 1|clear_bg); //dec
	lcd.drawInteger(5, 30, 1234, HEX, RGB(0,0,0), RGB(255,255,255), 1|clear_bg); //hex
	lcd.drawInteger(5, 40, 1234, OCT, RGB(0,0,0), RGB(255,255,255), 1|clear_bg); //oct
	lcd.drawInteger(5, 50, 1234, BIN, RGB(0,0,0), RGB(255,255,255), 1|clear_bg); //bin
*/

	//print
	lcd.setTextSize(1|clear_bg);
	lcd.setTextColor(RGB(200,0,0), RGB(255,255,255));
	lcd.setCursor(2, 60);
	lcd.print("String from RAM\nnext line1\n"); //string from ram
	lcd.println(12345);      //int
	lcd.println('A');        //char
	lcd.println(123456789L); //long
	lcd.println(98765.43);   //float/double
}

void change( uint8_t* v, bool* dir, uint8_t step)
{
	if( *dir )
	{
		*v = *v + step;
		if( *v > 200 )
			*dir = false;
	}
	else
	{
		*v = *v - step;
		if( *v < 50 )
			*dir = true;
	}
}
///////////////////////////////////////////////////////////////////////
// Scroll text forever
uint8_t r, g, b;
bool rb, gb, bb;
int _offset = -9999;

void loop()
{
	
	change( &r, &rb, random(2,30));
	change( &g, &gb, random(2,30));
	change( &g, &bb, random(2,30));
	
	//lcd.fillScreen(RGB(0,0,0));


	//const char text[] = "The quick brown fox jumps over the lazy dog. 0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	//const char text[] = "John loves Claudia...";
	const int textWidth = strlen(text) * FONT_WIDTH;

	if( _offset < -textWidth )
	{
		Serial.println("RESET offset");
		_offset = 32-7;
	}

	//Serial.printf("offset %d\r\n",_offset);


	lcd.drawText(_offset--, 0, text, RGB(r,g,b), RGB(0,0,0));
	delay(20);
}
