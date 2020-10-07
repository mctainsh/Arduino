///////////////////////////////////////////////////////////////////////////
// Reset the dis[play to start a new draw
void displaySetup( Adafruit_SSD1306* disp)
{
	disp->clearDisplay();
	disp->setTextSize(2);
	disp->setTextColor(SSD1306_WHITE);
	disp->setCursor(0, 0);
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

///////////////////////////////////////////////////////////////////////////////
// Draw a graph of the data
int _depthDrawn = DEPTH / 2;	// 12=(/2) or 24=(/1) hourts
float _minDraw, _maxDraw;		// Vertical ranges
void DisplayGraph( Adafruit_SSD1306 *disp, String title, float values[DEPTH+1])
{
	_minDraw =  999;
	_maxDraw = -999;
	for( int n = 0; n < _depthDrawn; n++)
	{
		if( isnan( values[n] ))
			continue;
		if( _minDraw > values[n] )
			_minDraw = values[n];
		if( _maxDraw < values[n] )
			_maxDraw = values[n];
	}

	// Write up the title and range
	disp->setTextSize(2);
	disp->printf( "%.0lf", max(_minDraw, _maxDraw));
	disp->setCursor(15,23);
	disp->println( title);
	disp->setCursor(0,46);
	disp->printf( "%.0f", min(_minDraw, _maxDraw));

	// Show nothing if no change
	if( _minDraw == _maxDraw )
		return;

	// Blank out background
	for( int n = 0; n < _depthDrawn; n++)
	{
		int x1 =W(n);
		int y1 = H(values[n]);
		disp->fillCircle( x1, y1, 2, SSD1306_BLACK);
	}

	// Draw the line
	int x0, y0 = INT_MAX;
	for( int n = 0; n < _depthDrawn; n++)
	{
		if( isnan( values[n] ))
		{
			y0 = INT_MAX;
			continue;
		}

		// Draw the line
		int x1 = W(n);
		int y1 = H(values[n]);
		if( y0 != INT_MAX )
			disp->drawLine(x0, y0,x1,y1, SSD1306_WHITE);
		x0 = x1;
		y0 = y1;
	}
}
int W(int n)
{
	return  WIDTH - (WIDTH*n/_depthDrawn);
}
int H(float v)
{
	return HEIGHT-(HEIGHT*(v-_minDraw)/(_maxDraw-_minDraw));
}
