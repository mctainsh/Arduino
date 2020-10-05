
////////////////////////////////////////////////////////////////////////////////////////
// Display the outside weather detail
bool _firstCall = true;
const char* host = "www.bom.gov.au";
int _nextMills = 0;
void displayOutsideWeather()
{
	displaySetup( &display2);

	// On first call display the local IP address
	if( _firstCall )
	{
		display2.println(WiFi.localIP());
		_firstCall = false;
		return;
	}

	// Read every 10 minutes
	if( !DEBUG && _nextMills <  millis() )
	{
		if( !ReadWebWeatherHistory() )
			return;
		_nextMills = millis() + 10 * 60 * 1000;
	}

	// Every 3 seconds we display a different screen
	switch( (millis() / 5000 )%4)
	{
		// Display the latest reading
		case 0:
		case 2:
			DisplayOutsideTempAndHumidity();
			break;
		case 1:
			DisplayGraph( &display2, "Temp", _air_temp);
			break;
		default:
			DisplayGraph( &display2, "Humidity", _rel_hum);
			break;
	}
	display2.display();
}


////////////////////////////////////////////////////////////////////////////////
// Show siomple current temp and humiddity
void DisplayOutsideTempAndHumidity()
{
	displaySetup(&display2);
	ShowWeather(&display2, _air_temp[0], _rel_hum[0]);
	display2.display();
}
