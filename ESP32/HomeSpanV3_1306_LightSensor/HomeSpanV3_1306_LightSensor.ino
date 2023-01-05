#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#include <HTTPClient.h>
#include <WiFi.h>

#include "DHT.h"

const char *ssid = "RhinoNBN";
const char *password = "DexterIs#1BadDog";

#include <Fonts/Picopixel.h>
#include <Fonts/FreeMono9pt7b.h>


// Roaming module
//#define SENSOR_ID 0
//#define SCREEN_HEIGHT 32

// Bedroom
#define SENSOR_ID 1
#define SCREEN_HEIGHT 64


#define SCREEN_WIDTH 128
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SDA_PIN 18
#define SCL_PIN 19
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_B 3		  // Blue. Near OLED
#define LED_G 5		  // Green. Near USB
#define LED_P 10	  // Yellow. Power. Near NeoPixel output
#define INPUT_LIGHT 4 // IO4

#define DHTPIN 2	  // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321

DHT _dht(DHTPIN, DHTTYPE);

void setup()
{
	// Setup serial ports
	Serial.begin(115200);
	Serial.println("Start V1.2");

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

	_dht.begin();

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

	digitalWrite(LED_B, LOW);
	delay(500);
	digitalWrite(LED_G, LOW);
	delay(500);
	digitalWrite(LED_P, LOW);

	Serial.println("Setup complete");
}

int _countGood = 0;
int _countBad = 0;

#define BUFFER_LEN 100
int _buffer[BUFFER_LEN];
int _bufferIndex = 0;

int _nextUpdate = 0;
#define UPDATE_INTERVAL 60

// Weather
char _humidityStr[32], _tempStr[32];
float _humidity;
float _tempC;

///////////////////////////////////////////////////////////////////////////////
// Main loop
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
	//display.setFont(&Picopixel);
	// display.getTextBounds(intStr, 0, 0, &x1, &y1, &w, &h);
	// display.setCursor(SCREEN_WIDTH - w - x1, h);
	display.setCursor(1, 1);
	display.setTextSize(1);
	display.print(intStr);

	// Draw the Temp and humidity
	_humidity = _dht.readHumidity();
	_tempC = _dht.readTemperature();

	if (isnan(_tempC))
		sprintf(_tempStr, "--°C");
	else
		sprintf(_tempStr, "%.1fC", _tempC);
	display.setCursor(1, 18);
	display.setTextSize(2);
	display.print(_tempStr);

	if (isnan(_humidity))
		sprintf(_humidityStr, "--");
	else
		sprintf(_humidityStr, "%.1f", _humidity);
	display.setCursor(70, 18);
	display.setTextSize(2);
	display.print(_humidityStr);

	Serial.print(F("Humidity: "));
	Serial.print(_humidity);
	Serial.print(F("%  Temperature: "));
	Serial.print(_tempC);
	Serial.print(F("°C "));
	Serial.println();

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
		display.setCursor(1, 1);

		// Reeboot if we loose wifi
		if (WiFi.status() != WL_CONNECTED)
			ESP.restart();

		char httpBuff[255];
		// 0 = Roaming
		// 1 = Bed room
		// sprintf(httpBuff, "http://192.168.1.7:488/api/weatherforecast/SaveLight/%d/0", mean);
		sprintf(httpBuff, "https://iSurv8.securehub.net/api/weatherforecast/SaveLight/%d/%d/%s/%s", mean, SENSOR_ID, _tempStr, _humidityStr);
		Serial.println(httpBuff);
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
		delay(5000);
	}

	// Draw analog value
	itoa(mean, intStr, 10);
	//display.setFont(&TomThumb);
	display.setTextSize(1);
	display.getTextBounds(intStr, 0, 0, &x1, &y1, &w, &h);
	display.setCursor(SCREEN_WIDTH - w - x1, 1);//SCREEN_HEIGHT - h - y1);
	display.print(intStr);

	// Update display
	display.display();
}