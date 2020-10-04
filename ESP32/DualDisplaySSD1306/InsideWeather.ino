
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

	// Start display refresh
	displaySetup(&display1);

	// Show the rotating indicator
	display1.print(TICK_ROLL[_busyCount%4]);
	display1.setCursor(116, 0);
	display1.print(TICK_ROLL[3-(_busyCount%4)]);
	_busyCount++;

	// Show the expanding bar
	display1.drawCircle(WIDTH/2, 7, 2*(_busyCount%24), SSD1306_WHITE);
	display1.drawCircle(WIDTH/2, 7, 2*((_busyCount-8)%24), SSD1306_WHITE);
	display1.drawCircle(WIDTH/2, 7, 2*((_busyCount-16)%24), SSD1306_WHITE);
	display1.fillRect(0, 16, WIDTH, HEIGHT, SSD1306_BLACK);

	// Show the real weather
	ShowWeather(&display1, _insideTemp, _insideHumidity);
	display1.display();
}
