#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>

//Constants
#define DHTPIN 17     					// what pin we're connected to
#define DHTTYPE DHT22   				// DHT 22  (AM2302)
#define OLED_RESET     	4 				// Reset pin # (or -1 if sharing Arduino reset pin)
#define WIDTH 			((int16_t)128) 	// OLED display width, in pixels
#define HEIGHT 			((int16_t)64) 	// OLED display height, in pixels
#define WIFI_SSID		"RhinoNBN"
#define WIFI_PWD		""
#define DEPTH			48
#define DEBUG			true

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
	delay(2000);
}

void loop()
{
	// Left display
	DisplayInsideWeather();
	delay(1000);

	// Right Diaplay
	if(WiFi.status() == WL_CONNECTED)
		displayOutsideWeather();
	else
		displayNetworks();

	// Invert and restore display, pausing in-between
	display1.invertDisplay(true);
	delay(1000);
	display1.invertDisplay(false);
	delay(10000);
}

////////////////////////////////////////////////////////////////////////////////////////
// No network so record some details
void displayNetworks()
{
	displaySetup( &display2 );
	display2.setTextSize(1);

	// Setup Wifi to scan mode
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	Serial.println("* Scan start");

	// WiFi.scanNetworks will return the number of networks found
	int n = WiFi.scanNetworks();
	Serial.println("* Scan done");
	display2.printf("Found %d networkd\r\n", n);
	for (int i = 0; i < n; ++i)
	{
		display2.println(WiFi.SSID(i));
		Serial.print("* ");
		Serial.println(WiFi.SSID(i));
		delay(10);
	}

	// Kick off a connection request
	WiFi.begin(WIFI_SSID, WIFI_PWD);
	display2.display();
}

////////////////////////////////////////////////////////////////////////////////////////
// Display the outdie weather detail
// Read data from JSON http://www.bom.gov.au/fwo/IDQ60901/IDQ60901.94576.json
// List of stations is at http://www.bom.gov.au/qld/observations/brisbane.shtml
bool _firstCall = true;
const char* host = "www.bom.gov.au";
int _nextMills = 0;
void displayOutsideWeather()
{
	// On first call display the local IP address
	if( _firstCall )
	{
		displaySetup( &display2);
		display2.println(WiFi.localIP());
		_firstCall = false;
		display2.display();
		return;
	}

	// Display the latest reading
	DisplayOutsideTempAndHumidity();

	// Read every 10 minutes
	if( _nextMills > millis() )
		return;
	_nextMills = millis() + 10 * 60 * 1000;

	displaySetup( &display2);
	// Read the weather
	WiFiClient client;
	const int httpPort = 80;
	if (!client.connect(host, httpPort))
	{
		display2.println("connection failed");
		display2.display();
		return;
	}

	// This will send the request to the server
	client.print(String("GET /fwo/IDQ60901/IDQ60901.94576.json HTTP/1.1\r\n") +
	             "Host: www.bom.gov.au\r\n" +
	             "Connection: keep-alive\r\n" +
	             "Cache-Control: max-age=0\r\n" +
	             "Upgrade-Insecure-Requests: 1\r\n" +
	             "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36\r\n" +
	             "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n" +
	             "Accept-Encoding: deflate\r\n" +
	             //"Accept-Encoding: gzip, deflate\r\n" +
	             "Accept-Language: en-GB,en-US;q=0.9,en;q=0.8\r\n" +
	             //"Cookie: __utma=172860464.1893302911.1601612971.1601612971.1601612971.1; __utmc=172860464; __utmz=172860464.1601612971.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __utmt=1; __utmb=172860464.13.9.1601613182969\r\n" +
	             "If-None-Match: \"a67bdc-1e434-5b0a89ad1fa80\"\r\n" +
	             "\r\n");
	//"If-Modified-Since: Fri, 02 Oct 2020 04:31:22 GMT\r\n\r\n");

	unsigned long timeout = millis();
	while (client.available() == 0)
	{
		if (millis() - timeout > 25000)
		{
			display2.println(">>> Client Timeout !");
			client.stop();
			return;
		}
	}

	// Read all the lines of the reply from server and print them to Serial
	// "apparent_t": 25.9,
	// "air_temp": 27.4,
	// "press": 1021.2,
	// "rel_hum": 38,
	// "wind_dir": "ENE",
	// "wind_spd_kmh": 11,
	int depth = 0;
	String air_temp = "";
	String rel_hum = "";
	while(client.available())
	{
		_jsonLine = client.readStringUntil(',');
		parseBOM( "\"air_temp\": ", &air_temp );
		parseBOM( "\"rel_hum\": ", &rel_hum );
		if( air_temp != "" && rel_hum != "" )
		{
			_air_temp[depth] = air_temp.toFloat();
			_rel_hum[depth] = rel_hum.toFloat();
			depth++;
			if( depth >= DEPTH )
				break;
			air_temp = "";
			rel_hum = "";
		}
		Serial.printf( "READLINE(%d) %s\r\n", millis(), _jsonLine.c_str());
	}
	client.stop();

	// Clean out the rest
	while(depth < DEPTH)
	{
		_air_temp[depth] = 0.0f;
		_rel_hum[depth] = 0.0f;
		depth++;
	}

	// Debug dump the data sets
	for( int n = 0; n < DEPTH; n++)
		Serial.printf( "%.1f, ", _air_temp[n]);	
	Serial.println();
	for( int n = 0; n < DEPTH; n++)
		Serial.printf( "%.1f, ", _rel_hum[n]);	

	DisplayOutsideTempAndHumidity();
}

////////////////////////////////////////////////////////////////////////////////
// Show siomple current temp and humiddity
void DisplayOutsideTempAndHumidity()
{
	displaySetup(&display2);
	ShowWeather(&display2, _air_temp[0], _rel_hum[0]);
	display2.display();
}

///////////////////////////////////////////////////////////////////////////
// Reset the dis[play to start a new draw
void displaySetup( Adafruit_SSD1306* disp)
{
	disp->clearDisplay();
	disp->setTextSize(2);
	disp->setTextColor(SSD1306_WHITE);
	disp->setCursor(0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
// Find the value after the field
//const char* parseBOM( const char* field )
//{
//	const char* line = _jsonLine.c_str();
//	const char *  i = strstr(line,field);
//	if ( i == NULL )
//		return NULL;
//	char * result = i + strlen(field);	// Get the field
//	result[strlen(result)-2] = NULL;			// Remove terminating comma
//	Serial.printf("Found %s in %s\r\n", result, line);
//	return result;
//}

//////////////////////////////////////////////////////////////////////////////////////
// Find the value after the field
void parseBOM( const char* field, String *output )
{
	const char* line = _jsonLine.c_str();
	const char *  i = strstr(line,field);
	if ( i == NULL )
		return;
	String result = i + strlen(field);	// Get the field
	//result[strlen(result)-2] = NULL;			// Remove terminating comma
	int comma = result.lastIndexOf(',');
	if( comma > 0 )
		result = result.substring(0, comma);
	Serial.printf("Found %s in %s\r\n", result.c_str(), line);
	if( output->length() < 1 )
		output->concat( result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Display local Temp and Humidity
void DisplayInsideWeather()
{
	if( _nextInsideRead < millis() )
	{
		_insideTemp = dht.readTemperature();
		_insideHumidity = dht.readHumidity();
		_nextInsideRead =  millis() + 10 * 1000;
		Serial.printf("Temp:%f, Humidity:%f%%\r\n", _insideTemp, _insideHumidity);
	}

	displaySetup(&display1);
	ShowWeather(&display1, _insideTemp, _insideHumidity);
	display1.display();
}

///////////////////////////////////////////////////////////////////////////////
// Show thr readings
void ShowWeather(Adafruit_SSD1306 *disp, float temperature, float humidity )
{
	disp->setTextSize(4);

	// Readings
	disp->setCursor(0, 20);
	if( !isnan(temperature) )
		disp->printf("%.0f", temperature);
	disp->setCursor(65, 20);
	if( !isnan(humidity))
		disp->printf("%.0f", humidity);

	// Units
	disp->setTextSize(2);
	disp->setCursor(47, 20);
	disp->print("C");
	disp->setCursor(114, 20);
	disp->print("%");

	// Sub titles
	disp->setTextSize(1);
	disp->setCursor(0, 56);
	disp->print(" Temp.");
	disp->setCursor(65, 56);
	disp->print("Humidity");
}


//////////////////////////////////////////////////////////////////////
// Scan I2C for devices
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
