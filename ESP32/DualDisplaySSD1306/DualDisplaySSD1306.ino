//////////////////////////////////////////////////////////////////////////////////////
// NOTES:
// 1: Set WIFI SSID and password below
// 2: Setup
//			board		ESP32 WROOM-DA Module
// 			port 		/dev/cu.usbserial-0001
//			CPU Freq	160 (240 worked)
// 3: Press and hold BOOT button until download starts
//////////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
//#include <Adafruit_Sensor.h>

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <WiFi.h>

//Constants
#define DHTPIN 17     					// what pin we're connected to
#define DHTTYPE DHT22   				// DHT 22  (AM2302)
#define OLED_RESET     	4 				// Reset pin # (or -1 if sharing Arduino reset pin)
#define WIDTH 			((int16_t)128) 	// OLED display width, in pixels
#define HEIGHT 			((int16_t)64) 	// OLED display height, in pixels
//#define WIFI_SSID		"RhinoNBN"
//#define WIFI_PWD		"**************"
#define DEPTH			48
#define DEBUG			false
//#define NaN				(0/0)			// Not a number
#define NaN				(-100)			// Not a number

//Variables
DHT dht(DHTPIN, DHTTYPE); 	// Initialize DHT sensor for normal 16mhz Arduino
int chk;
String _jsonLine;

// Local weather
float _insideTemp;
float _insideHumidity;
int _nextInsideRead;

// Outside weather (Debugging)
float _air_temp[DEPTH+1] = { 23.7, 24.7, 24.3, 25.4, 25.5, 25.1, 25.9, 26.7, 26.7, 27.3, 26.9, 26.6, 26.2, 26.2, 24.3, 24.5, 22.2, 21.5, 20.3, 19.2, 18.3, 17.4, 16.8, 17.1, 17.0, 17.3, 17.5, 17.6, 18.1, 18.6, 19.1, 19.3, 19.5, 19.4, 19.3, 19.8, 20.1, 20.2, 21.0, 21.3, 20.9, 21.2, 21.4, 21.6, 22.0, 22.8, 23.9, 25.1 };
float _rel_hum [DEPTH+1] = { 51.0, 49.0, 49.0, 48.0, 47.0, 49.0, 43.0, 43.0, 42.0, 41.0, 42.0, 42.0, 46.0, 48.0, 55.0, 57.0, 60.0, 62.0, 65.0, 71.0, 75.0, 80.0, 81.0, 80.0, 81.0, 80.0, 83.0, 83.0, 83.0, 83.0, 83.0, 82.0, 83.0, 85.0, 84.0, 84.0, 84.0, 83.0, 81.0, 81.0, 81.0, 81.0, 79.0, 77.0, 72.0, 60.0, 53.0, 46.0 };

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display1(WIDTH, HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(WIDTH, HEIGHT, &Wire, OLED_RESET);

// Annimation
int _busyCount = 0;
const char* TICK_ROLL = "\\|/-";

void setup()
{
	Serial.begin(115200);
	//ScanI2C();
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
	delay(2000);
}

//void configModeCallback (WiFiManager *myWiFiManager) {
//  Serial.println("Entered config mode");
//  Serial.println(WiFi.softAPIP());
//  Serial.println(myWiFiManager->getConfigPortalSSID());
//}

////////////////////////////////////////////////////////////////////////////////////////
// Setup WIFI Credentials
//void SetupWifiCredentials()
//{
//	Serial.println("Setup WIFI");
//	wifiManager.startConfigPortal("DaWeatherStation");
//	wifiManager.setAPCallback(configModeCallback);
//	Serial.println("connected...yeey :)");
//}

////////////////////////////////////////////////////////////////////////////////////////
// Main loop
void loop()
{
	// Left display
	DisplayInsideWeather();

	// Right Diaplay
	if(DEBUG || WiFi.status() == WL_CONNECTED)
		displayOutsideWeather();
	else
		displayNetworks();

	// Invert and restore display, pausing in-between
	//display1.invertDisplay(true);
	//delay(1000);
	//display1.invertDisplay(false);
	//delay(10000);
}
