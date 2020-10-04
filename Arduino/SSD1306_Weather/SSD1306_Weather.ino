// Display stuff
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Weather stuff
#include <dht11.h>
#define DHT11PIN 4
dht11 DHT11;

#define PIN_LED 13

#define SERIAL_ON false   // Serial IO on or off

void setup() 
{
  // Setup blike
  pinMode(PIN_LED, OUTPUT);

  // Comms
  Serial.begin(9600);

  // Display setup
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { 
    // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println(F("SSD1306 allocation OK"));

  // Show initial display buffer contents on the screen the library initializes this with an Adafruit splash screen.
  display.display();
  delay(500); 

  // Clear the buffer
  display.clearDisplay();
}

int busyCount = 0;
const char* TICK_ROLL = "\\|/-";
void loop() 
{
  // Start display refresh
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Show the rotating indicator
  display.setTextSize(2);          
  display.setCursor(0, 0);
  display.print(TICK_ROLL[busyCount%4]);
  display.setCursor(116, 0);
  display.print(TICK_ROLL[busyCount%4]);
  busyCount++;

  // Show the expanding bar
  display.drawCircle(display.width()/2, 7, 2*(busyCount%24), SSD1306_WHITE);
  display.drawCircle(display.width()/2, 7, 2*((busyCount-8)%24), SSD1306_WHITE);
  display.drawCircle(display.width()/2, 7, 2*((busyCount-16)%24), SSD1306_WHITE);
  display.fillRect(0, 16, display.width(), display.height(), SSD1306_BLACK);
  
  if( busyCount % 10 == 0 )
  {
    ReadWeather();
  }

  ShowWeather();
  display.display();
  

  // Blink LED
  //for(int n = 100; n < 1000; n+=100 )
  int n = 10;
  {
    if(SERIAL_ON)
        Serial.print("\nSpeed");
    Serial.print(n);
    digitalWrite(PIN_LED,HIGH);
    delay(n);
    digitalWrite(PIN_LED,LOW);
    delay(n);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Read the weather
void ReadWeather()
{
  // Read the value
  int chk = DHT11.read(DHT11PIN);
  if(SERIAL_ON)
  {
    Serial.print("Humidity (%): ");
    Serial.println((float)DHT11.humidity, 2);

    Serial.print("Temperature (C): ");
    Serial.println((float)DHT11.temperature, 2);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Show thr readings
void ShowWeather()
{
  // Read the value
  int chk = DHT11.read(DHT11PIN);

  display.setCursor(10, 0);

  display.setTextSize(4);          

  display.setCursor(0, 20);
  display.print((int)DHT11.temperature);
  
  display.setCursor(65, 20);
  display.print((int)DHT11.humidity);
  
  // Units
  display.setTextSize(2);          
  display.setCursor(47, 20);
  display.print(F("C"));
  display.setCursor(114, 20);
  display.print(F("%"));

  // Titles
  display.setTextSize(1);          
  display.setCursor(0, 56);
  display.print(F("  Temp.    Humidity"));  
}
