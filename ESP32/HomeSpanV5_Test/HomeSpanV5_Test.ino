///////////////////////////////////////////////////////////////////////////////
// This sketch is to test the HomeSpan V5 board

// Enable hardware logging
#include "esp32-hal-log.h"

// Add support for Serial2 (The USB Serial Port)
#include "HardwareSerial.h"
HardwareSerial SerialUsb(2);

// Display libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

// Display settings
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32?
#define OLED_RESET -1
#define SDA_PIN 3
#define SCL_PIN 2
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define LEDS
#define LED_B 10 	// Blue LED near wifi antenna
#define LED_G 5 	// Green power LED nead USB Connector


// the setup function runs once when you press reset or power the board
void setup()
{
	// Initialize digital pin LED_B as an output.
	pinMode(LED_B, OUTPUT); 
	digitalWrite(LED_B, HIGH);				   
	pinMode(LED_G, OUTPUT);

	// Setup serail communications
	Serial.begin(115200);
	SerialUsb.begin(115200);
	SerialUsb.setDebugOutput(true);

	// Turn on OLED
	Wire.setPins(SDA_PIN, SCL_PIN);
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
	{
		log_e(" ***************** SSD1360 Failed");
		digitalWrite(LED_B, HIGH);
		delay(500);
		digitalWrite(LED_B, LOW);
		delay(1000);
	}
	display.display();
	delay(200);
	display.clearDisplay();
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(20, 20);
	display.print("Ready");
	display.display();
}

int n = 0;
// the loop function runs over and over again forever
void loop()
{
	n++;

	//log_v("Verbose %d", n);
	//log_d("Debug");
	// Check the debug level that is turned on
	log_i("Info %d", n);
	log_w("Warning");
	log_e("Error");

	digitalWrite(LED_B, HIGH); 	delay(500);				   
	digitalWrite(LED_G, LOW);  	delay(100);				   
	digitalWrite(LED_G, HIGH); 	delay(100);				   
	digitalWrite(LED_G, LOW);  	delay(100);				   
	digitalWrite(LED_G, HIGH); 	delay(100);				   
	digitalWrite(LED_B, LOW);  	delay(250);				   
	
	Serial.printf("Hello! %d\r\n", n);

	char intStr[64];
	sprintf(intStr, "H:%d", n);
	display.clearDisplay();
	display.setTextColor(SSD1306_WHITE);
	display.setTextSize(2);
	display.setCursor(1, 1);
	display.print(intStr);
	display.display();
}
