/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int sensorPin = A0;
int sensorValue;

void setup()
{
	myservo.attach(9);  // attaches the servo on pin 9 to the servo object
	Serial.begin(115200);
}

void loop()
{
	sensorValue = analogRead(sensorPin);
	int output = map(sensorValue, 0, 902, 0, 180);
	// change the analog out value:
	//analogWrite(analogOutPin, outputValue);
	//if( output < 0 )
	//	output *= -1;
	//output = (int)(output/10)*10;
	myservo.write(output);

	Serial.print("sensor = ");
	Serial.print(sensorValue);
	Serial.print("\t output = ");
	Serial.println(output);

	delay(50);
	return;
	for (pos = 0; pos <= 180; pos += 1)   // goes from 0 degrees to 180 degrees
	{
		// in steps of 1 degree
		myservo.write(pos);              // tell servo to go to position in variable 'pos'
		delay(15);                       // waits 15ms for the servo to reach the position
	}
	for (pos = 180; pos >= 0; pos -= 1)   // goes from 180 degrees to 0 degrees
	{
		myservo.write(pos);              // tell servo to go to position in variable 'pos'
		delay(15);                       // waits 15ms for the servo to reach the position
	}
}
