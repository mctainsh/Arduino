/*
  Keyboard Message test

  For the Arduino Leonardo and Micro.

  Sends a text string when a button is pressed.

  The circuit:
  - pushbutton attached from pin 0 to ground
  - 10 kilohm resistor attached from pin 0 to +5V

  created 24 Oct 2011
  modified 27 Mar 2012
  by Tom Igoe
  modified 11 Nov 2013
  by Scott Fitzgerald

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/KeyboardMessage
*/

void ProcessCommandPair(char key2);

// Define the number of rows and columns of the keypad
const byte ROWS = 4;  // four rows
const byte COLS = 4;  // four columns

// Define the symbols on the buttons of the keypad
const char KEY_MATRIX[ROWS][COLS] = {
	{ '1', '2', '3', 'A' },
	{ '4', '5', '6', 'B' },
	{ '7', '8', '9', 'C' },
	{ '*', '0', '#', 'D' }
};

// Last key pressed
char _firstKey = 0;

// Time of the last keypress
int32_t _lastKeyPress = 0;

// Last state of PCB button
int _previousPcbBtnState = HIGH;

// Assign pins to rows
// >>> 4 x 4
//byte _rowPins[ROWS] = { 1, 2, 4, 6 };
//byte _colPins[COLS] = { 8, 10, 13, 14 };

//  >>> 4 x 3
byte _rowPins[ROWS] = { 10, 1, 2, 6 };
byte _colPins[COLS] = { 8, 13, 4 };

#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;  // USB Keypoard output

const int PCB_BTN = 0;	// input pin for pushbutton

////////////////////////////////////////////////////////////////////////////////////
// Setup the outputs
void setup()
{
	// Startup blink
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
	delay(200);

	// Make the pushButton pin an input:
	pinMode(PCB_BTN, INPUT_PULLUP);

	// Initialize row pins as inputs with pull-up resistors
	for (byte i = 0; i < ROWS; i++)
		pinMode(_rowPins[i], INPUT_PULLUP);

	// Initialize column pins as outputs
	for (byte j = 0; j < COLS; j++)
	{
		pinMode(_colPins[j], OUTPUT);
		digitalWrite(_colPins[j], HIGH);
	}

	// initialize control over the keyboard:
	Keyboard.begin();
	USB.begin();

	// Rady blink
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
}

//////////////////////////////////////////////////////////////////////////
// Loop forever
void loop()
{
	// Handy testing function without keypad attached
	int pcbBtnState = digitalRead(PCB_BTN);
	if (pcbBtnState != _previousPcbBtnState)
	{
		if (pcbBtnState == LOW)
			Keyboard.print("You pressed TEST button ");
		else
			Keyboard.print("RELEASED");
		digitalWrite(LED_BUILTIN, !pcbBtnState);
		_previousPcbBtnState = pcbBtnState;
	}

	// Scan each column
	for (byte j = 0; j < COLS; j++)
	{
		// Set the current column to LOW
		digitalWrite(_colPins[j], LOW);

		// Check each row
		for (byte i = 0; i < ROWS; i++)
		{
			if (digitalRead(_rowPins[i]) == LOW)
			{
				// Debounce
				delay(50);
				if (digitalRead(_rowPins[i]) == LOW)
				{
					// Blink
					digitalWrite(LED_BUILTIN, HIGH);
					ProcessCommandPair(KEY_MATRIX[i][j]);

					// Wait until the key is released
					while (digitalRead(_rowPins[i]) == LOW)
					{
					}

					digitalWrite(LED_BUILTIN, LOW);
				}
			}
		}

		// Set the column back to HIGH
		digitalWrite(_colPins[j], HIGH);
	}
}

void Backspace(int count)
{
	for (int n = 0; n < count; n++)
		Keyboard.write(KEY_BACKSPACE);
}

////////////////////////////////////////////////////////////////////////////////////////
// Process. First key must be C
void ProcessCommandPair(char key2)
{
	// Check if its meen a while since key press
	if ((millis() - _lastKeyPress) > 30000)
		_firstKey = 0;
	_lastKeyPress = millis();

	// Check first charactor
	Keyboard.print(key2);
	if (_firstKey == 0)
	{
		_firstKey = key2;
		return;
	}

	// Capture the first key command
	if (_firstKey != '*')
	{
		_firstKey = 0;
		return;
	}

	_firstKey = 0;
	switch (key2)
	{
		case '1':
			Backspace(25);
			Keyboard.print("Lunch_1313131");
			Keyboard.write(KEY_RETURN);
			break;
		case '2':
			Backspace(25);
			Keyboard.print("Batw1ngs-Adm1n1!");
			Keyboard.write(KEY_RETURN);
			break;
		case '3':
			Backspace(25);
			Keyboard.print("Derek");
			Keyboard.write(KEY_TAB);
			Keyboard.print("Motorola12");
			Keyboard.write(KEY_RETURN);
			break;
		case '4':
			Backspace(25);
			Keyboard.print("Dexter@7878");
			Keyboard.write(KEY_RETURN);
			break;}
}
