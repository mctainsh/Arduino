#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#include <HTTPClient.h>
#include <WiFi.h>

const char *ssid = "RhinoNBN";
const char *password = "DexterIs#1BadDog";

#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono24pt7b.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SDA_PIN 18
#define SCL_PIN 19
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_B 3		  // Blue. Near OLED
#define LED_G 5		  // Green. Near USB
#define LED_P 10	  // Yellow. Power. Near NeoPixel output
#define INPUT_LIGHT 4 // IO4

void setup()
{
	// Setup serial ports
	Serial.begin(115200);
	Serial.println("Start V1.1");

	WiFi.begin(ssid, password);

	// Turn on LEDS
	pinMode(LED_B, OUTPUT);
	pinMode(LED_G, OUTPUT);
	pinMode(LED_P, OUTPUT);
	digitalWrite(LED_B, HIGH);
	digitalWrite(LED_G, HIGH);
	digitalWrite(LED_P, HIGH);

	// Turn on OLED
	Wire.setPins(SDA_PIN, SCL_PIN);
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
	{
		Serial.println("SSD1360 Failed");
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
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		display.print(".");
		display.display();
		Serial.println("Connecting to WiFi..");
	}
	display.print("Connected");

	Serial.println("Connected to the WiFi network");

	Serial.println("Setup complete");
}

int _countGood = 0;
int _countBad = 0;

#define BUFFER_LEN 100
int _buffer[BUFFER_LEN];
int _bufferIndex = 0;

int _nextUpdate = 0;
#define UPDATE_INTERVAL 60

void loop()
{
	int16_t x1, y1;
	uint16_t w, h;
	char intStr[64];

	// delay(100);
	display.clearDisplay();

	// Send if we have new data
	int timeSinceSend = (millis() - _nextUpdate) / 1000;
	bool sendData = _nextUpdate > millis() || timeSinceSend > UPDATE_INTERVAL;

	// Draw count
	sprintf(intStr, "%d:%d:%d", UPDATE_INTERVAL - timeSinceSend, _countGood, _countBad);
	display.setFont(&FreeMono12pt7b);
	display.getTextBounds(intStr, 0, 0, &x1, &y1, &w, &h);
	display.setCursor(SCREEN_WIDTH - w - x1, h);
	// display.setCursor(SCREEN_WIDTH - w, h);
	display.print(intStr);
	// display.printf("x:%d W%d\r\n" , x1, w);
	// display.printf("y:%d H%d\r\n" , y1, h);

	// Average the value
	_bufferIndex++;
	if (0 > _bufferIndex || _bufferIndex >= BUFFER_LEN)
		_bufferIndex = 0;
	_buffer[_bufferIndex] = analogRead(INPUT_LIGHT);
	long total = 0;
	for (int n = 0; n < BUFFER_LEN; n++)
		total += _buffer[n];
	int mean = total / BUFFER_LEN;

	// Send every time we hit zero
	if (sendData)
	{
		_nextUpdate = millis();
		display.clearDisplay();
		display.setCursor(10, 15);

		char httpBuff[255];
		sprintf(httpBuff, "http://192.168.1.7:488/api/weatherforecast/SaveLight/%d/1", mean);
		HTTPClient http;
		http.begin(httpBuff); // Specify the URL
		int httpCode = http.GET();
		if (httpCode > 0)
		{ // Check for the returning code
			_countGood++;
			String payload = http.getString();
			Serial.println(httpCode);
			Serial.println(payload);
			display.print(payload);
		}
		else
		{
			_countBad++;
			Serial.println("Error on HTTP request");
			display.print("Error on HTTP");
		}
		display.display();
		http.end(); // Free the resources

		// display.setCursor(SCREEN_WIDTH - w, h);
		delay(2000);
	}

	// Draw analog value
	itoa(mean, intStr, 10);
	display.setFont(&FreeMono24pt7b);
	display.getTextBounds(intStr, 0, 0, &x1, &y1, &w, &h);
	display.setCursor(SCREEN_WIDTH - w - x1, SCREEN_HEIGHT - h - y1);
	display.print(intStr);

	// Update display
	display.display();
}