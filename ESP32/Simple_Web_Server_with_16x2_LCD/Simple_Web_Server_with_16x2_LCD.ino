#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <LiquidCrystal_I2C.h>

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "RhinoNBN";
const char* password = "";

WebServer server(80);

const int led = 2;

void handleRoot() {
  digitalWrite(led, 1);

  String body = "hello from esp8266!";
  body += millis();
  server.send(200, "text/plain", body );
  digitalWrite(led, 0);
}

void handleNotFound()
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  lcd.clear();
  for (uint8_t i = 0; i < server.args(); i++)
  {
    lcd.setCursor(0, i);
    lcd.print(server.argName(i));
    lcd.print(":");
    lcd.print(server.arg(i));
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void)
{
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // Blink the Pin
  lcd.print("4");
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  delay(250);
  lcd.print("3");
  digitalWrite(led, 0);
  delay(200);
  lcd.print("2");
  digitalWrite(led, 1);
  delay(1000);
  digitalWrite(led, 0);
  delay(200);
  lcd.print("1");
  digitalWrite(led, 1);
  delay(250);
  digitalWrite(led, 0);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SSID:");
  lcd.print(ssid);
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  if (MDNS.begin("esp32"))
    Serial.println("MDNS responder started");

  server.on("/", handleRoot);

  server.on("/inline", []()
  {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Main loop here. Try to do the reading and display now
int tick = 0;
const char *TICKS = "-=+*";
int currentTick;
void loop(void)
{
  //  lcd.clear();
  tick++;
  int newTick = ( tick / 50000 ) % 4;
  if ( newTick != currentTick )
  {
    currentTick = newTick;
    lcd.setCursor(15, 0);
    lcd.print(TICKS[newTick]);
  }
  server.handleClient();
}
