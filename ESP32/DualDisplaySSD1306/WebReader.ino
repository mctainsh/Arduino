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
	delay(2000);
}


/////////////////////////////////////////////////////////////////////////////////////
// Load the latest weather history from the web
// Read data from JSON http://www.bom.gov.au/fwo/IDQ60901/IDQ60901.94576.json
// List of stations is at http://www.bom.gov.au/qld/observations/brisbane.shtml
bool ReadWebWeatherHistoryJSON()
{
	// Read the weather
	WiFiClient client;
	const int httpPort = 80;
	if (!client.connect(host, httpPort))
	{
		display2.println(">> Connection failed !");
		display2.display();
		return false;
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
	             //"If-Modified-Since: Fri, 02 Oct 2020 04:31:22 GMT\r\n" +
	             "\r\n");

	unsigned long timeout = millis();
	while (client.available() == 0)
	{
		if (millis() - timeout > 25000)
		{
			display2.println(">>> Client Timeout !");
			display2.display();
			client.stop();
			return false;
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
		if( DEBUG )
			Serial.printf( "READLINE '%s'", millis(), _jsonLine.c_str());
	}
	client.stop();

	// Clean out the rest
	while(depth < DEPTH)
	{
		_air_temp[depth] = NaN;
		_rel_hum[depth] = NaN;
		depth++;
	}

	// Debug dump the data sets
	if( DEBUG )
	{
		for( int n = 0; n < DEPTH; n++)
			Serial.printf( "%.1f, ", _air_temp[n]);
		Serial.println();
		for( int n = 0; n < DEPTH; n++)
			Serial.printf( "%.1f, ", _rel_hum[n]);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Altername method from above to read as comma delimited data
//	http://www.bom.gov.au/fwo/IDQ60901/IDQ60901.94576.axf
// 	[data]
// 	sort_order,wmo,name[80],history_product[80],local_date_time[80],local_date_time_full[80],aifstime_utc[80],lat,lon,apparent_t,cloud[80],cloud_base_m,cloud_oktas,cloud_type_id,cloud_type[80],delta_t,gust_kmh,gust_kt,air_temp,dewpt,press,press_qnh,press_msl,press_tend[80],rain_trace[80],rel_hum,sea_state[80],swell_dir_worded[80],swell_height,swell_period,vis_km[80],weather[80],wind_dir[80],wind_spd_kmh,wind_spd_kt
// 	0,94576,"Brisbane","IDQ60901","06/12:00pm","20201006120000","20201006020000",-27.5,153.0,26.0,"-",-9999,-9999,-9999,"-",8.4,9,5,26.3,11.6,1019.7,1019.7,1019.7,"-","0.0",40,"-","-",-9999.0,-9999,"-","-","N",4,2
// 	1,94576,"Brisbane","IDQ60901","06/11:30am","20201006113000","20201006013000",-27.5,153.0,24.7,"-",-9999,-9999,-9999,"-",7.7,13,7,25.3,11.8,1020.3,1020.3,1020.3,"-","0.0",43,"-","-",-9999.0,-9999,"-","-","NE",6,3
bool ReadWebWeatherHistory()
{
	// Clean out the rest
	for( int n = 0; n < DEPTH; n++ )
		_air_temp[n] = _rel_hum[n] = NaN;

	// Read the weather
	WiFiClient client;
	const int httpPort = 80;
	if (!client.connect(host, httpPort))
	{
		display2.println(">> Connection failed !");
		display2.display();
		return false;
	}

	// This will send the request to the server
	client.print(String("GET /fwo/IDQ60901/IDQ60901.94576.axf HTTP/1.1\r\n") +
	             "Host: www.bom.gov.au\r\n" +
	             "Connection: keep-alive\r\n" +
	             "Cache-Control: max-age=0\r\n" +
	             "Upgrade-Insecure-Requests: 1\r\n" +
	             "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36\r\n" +
	             "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n" +
	             "Accept-Encoding: deflate\r\n" +
	             "Accept-Language: en-GB,en-US;q=0.9,en;q=0.8\r\n" +
	             "If-None-Match: \"a67bdc-1e434-5b0a89ad1fa80\"\r\n" +
	             "\r\n");

	unsigned long timeout = millis();
	while (client.available() == 0)
	{
		if (millis() - timeout > 25000)
		{
			display2.println(">>> Client Timeout !");
			display2.display();
			client.stop();
			return false;
		}
	}

	// Read until your get [data]
	String line;
	while(client.available())
	{
		line = client.readStringUntil('\n');
		if( line.startsWith("sort_order") )
			break;
		Serial.printf( "Skipping '%s'\r\n", line.c_str());
	}

	// Process each line to get field
	int depth = 0;
	String strIn;
	const int BUFFLEN = 512;
	char row[BUFFLEN+1];
	const char s[2] = ",";
	while(client.available())
	{
		strncpy (row, client.readStringUntil('\n').c_str(), BUFFLEN);
		
		// Split the row
		int column = 0;
		char *str = strtok( row, s);
		while ( str != NULL)
		{
			if( DEBUG )
				Serial.printf( "L%d C%d '%s'\r\n",  depth, column, str);
			if( column == 18 )
				_air_temp[depth] = atof(str);
			if( column == 25 )
			{
				_rel_hum[depth] = atof(str);
				break;
			}

			// Next column
			str = strtok( NULL, s);
			column++;
		}

		// New row
		depth++;
		if( depth >= DEPTH )
			break;
	}
	client.stop();

	// Debug dump the data sets
	if( DEBUG )
	{
		for( int n = 0; n < DEPTH; n++)
			Serial.printf( "%.1f, ", _air_temp[n]);
		Serial.println();
		for( int n = 0; n < DEPTH; n++)
			Serial.printf( "%.1f, ", _rel_hum[n]);
	}
	return true;
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
