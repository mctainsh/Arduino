/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

#define D1_PWR 9   // * 12  	// Header pin 9
#define D2_PWR 10  // * 11		// Header pin 10
#define DOT_PWR 15 // * (Or 1, 14, 15, 17) * 9		// Header pin 11
#define D3_PWR 3   // * 7		// Header pin 12
#define D4_PWR 2   // * 5		// Header pin 13

// 3, 6 unused
#define SEG_TR 6   // * 2
#define SEG_T 8	   // * 33
#define SEG_TL 7   // * 39
#define SEG_M 12   // * 10
#define SEG_BR 14  // * 35
#define SEG_BL 13  // * 13
#define SEG_B 5	   // * 4
#define DOT_TOP 11 // *
#define DOT_BOT 4  // * 6

#define PIN_LEN 5 + 7 + 2
int PINS[] = {D1_PWR, D2_PWR, DOT_PWR, D3_PWR, D4_PWR, SEG_TR,	SEG_T,
			  SEG_TL, SEG_M,  SEG_BR,  SEG_BL, SEG_B,  DOT_BOT, DOT_TOP};

#define SEGMENT_LENGTH 7
// Position of each segment
int POSITIONS[SEGMENT_LENGTH] = {SEG_TL, SEG_T, SEG_TR, SEG_M, SEG_BL, SEG_B, SEG_BR};

// Segements that are on for each digit
int NUMBERS[10][SEGMENT_LENGTH] = {
	//
	{1, 1, 1, 0, 1, 1, 1}, // 0
	{0, 0, 1, 0, 0, 0, 1}, // 1
	{0, 1, 1, 1, 1, 1, 0}, // 2
	{0, 1, 1, 1, 0, 1, 1}, // 3
	{1, 0, 1, 1, 0, 0, 1}, // 4
	{1, 1, 0, 1, 0, 1, 1}, // 5
	{1, 1, 0, 1, 1, 1, 1}, // 6
	{0, 1, 1, 0, 0, 0, 1}, // 7
	{1, 1, 1, 1, 1, 1, 1}, // 8
	{1, 1, 1, 1, 0, 1, 1}, // 9
};


// int PINS[] =  { 2, 3, 4, 6, 8, 10, 13, 33, 35, 39 };

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	// while (!Serial) {} // wait for serial port to connect. Needed for native USB port only
	Serial.println("Starting ...");

	// initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
	for (int n = 0; n < PIN_LEN; n++)
		pinMode(PINS[n], OUTPUT);

	// for (int n = 0; n < PIN_LEN; n++)
	//	pinMode(PINS[n], OUTPUT);

	Serial.println("I/O Setup ...");

	// Turn all on
	digitalWrite(LED_BUILTIN, HIGH);
	for (int n = 0; n < PIN_LEN; n++)
		digitalWrite(PINS[n], HIGH);
	delay(500);

	// Turn all off
	digitalWrite(LED_BUILTIN, LOW);
	for (int n = 0; n < PIN_LEN; n++)
		digitalWrite(PINS[n], LOW);

	testSegments();
}

int _segmentFinishTime = 0;
int _oldDigit = -1;
// Flash the digit
void ShowDigit(int digit, int number)
{
	if (0 > number || number > 9)
		return;

	// Wait for the segment to complete baking
	while (millis() < _segmentFinishTime)
		delay(1);
	if (_oldDigit != -1)
		digitalWrite(_oldDigit, LOW); // Off Digit

	// Turn ON/OFF the required segments
	for (int n = 0; n < SEGMENT_LENGTH; n++)
		digitalWrite(POSITIONS[n], (NUMBERS[number][n] ? HIGH : LOW));
	digitalWrite(digit, HIGH); // Power digit
	_oldDigit = digit;
	_segmentFinishTime = millis() + 6;

	// Turn off segments
	//	for (int n = 0; n < SEGMENT_LENGTH; n++)
	//		digitalWrite(POSITIONS[n], LOW);
	//	delay(1);
}

void ShowNumber(int number)
{
	ShowDigit(D4_PWR, number % 10);
	ShowDigit(D3_PWR, (number / 10) % 10);
	ShowDigit(D2_PWR, (number / 100) % 10);
	ShowDigit(D1_PWR, (number / 1000) % 10);
}

// the loop function runs over and over again forever
long count = 0;
void loop()
{
	Serial.println("Loop.");
	Serial.print(count);

	count++;
	int mod = millis() / 1000 % 2;
	digitalWrite(DOT_PWR, HIGH);
	digitalWrite(DOT_TOP, mod == 0 ? LOW : HIGH);
	digitalWrite(DOT_BOT, mod == 0 ? HIGH : LOW);

	ShowNumber(count / 10);
	count++;

	//	for( int n = 0; n < 10; n++ )
	//	{
	//		ShowNumber( D1_PWR, n );
	//		delay(500);   // wait for a second
	//	}
	// digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
	// digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
	// for( int n = 0; n < PIN_LEN; n++ )
}

/////////////////////////////////////////////////////////////////////////////////////
// Zig zag through all dots
void testSegments()
{
	int SEG_LEN = 5;
	int SEGS[SEG_LEN] = {D1_PWR, D2_PWR, DOT_PWR, D3_PWR, D4_PWR};
	for (int s = 0; s < SEG_LEN; s++)
	{
		int seg = SEGS[s];
		Serial.printf("Segment %d\r\n", seg);
		blink();
		for (int n = 0; n < SEGMENT_LENGTH; n++)
		{
			int pin = POSITIONS[n];

			if (seg == DOT_PWR)
				pin = n < 4 ? DOT_TOP : DOT_BOT;

			digitalWrite(seg, HIGH);
			digitalWrite(pin, HIGH);
			delay(20);
			digitalWrite(pin, LOW);
			digitalWrite(seg, LOW);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// Drive pins on
void testAllPins()
{
	for (int n = 0; n < 17; n++)
		pinMode(n, OUTPUT);

	for (int s = 0; s < 3; s++)
	{
		int seg = 15;
		pinMode(seg, OUTPUT);
		delay(300);
		for (int pin = 1; pin < 17; pin++)
		// for (int pin = 13; pin < 17; pin++)
		{
			pin = 12;
			pinMode(pin, OUTPUT);
			digitalWrite(seg, HIGH);
			digitalWrite(pin, HIGH);
			delay(100);
			digitalWrite(pin, LOW);
			digitalWrite(seg, LOW);
		}
	}
}

bool _blinkOn = true;
void blink()
{
	_blinkOn = !_blinkOn;
	//	digitalWrite(LED_BUILTIN, _blinkOn ? HIGH : LOW);
}
