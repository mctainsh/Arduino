/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BLUE is set to
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
#define LED_BLUE 10
#define LED_GREEN 5

int _loop = 0;
// the setup function runs once when you press reset or power the board
void setup() {

  // initialize digital pin LED_BLUE as an output.
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  Serial.begin(115200);

  Serial.printf("Internal heap\n");
  Serial.printf("  Total       %.2fKb\n", ESP.getHeapSize()/1000.0);
  Serial.printf("  Free        %.2fkb\n", ESP.getFreeHeap()/1000.0);
  Serial.printf("SPIRam\n");
  Serial.printf("  Total heap  %d\n", ESP.getPsramSize());
  Serial.printf("  Free Heap   %d\n", ESP.getFreePsram());
  Serial.printf("Flash\n");
  Serial.printf("  Sketch size %.2fMb\n", ESP.getSketchSize()/1000000.0);
  Serial.printf("  Sketch free %.2fMb\n", ESP.getFreeSketchSpace()/1000000.0);
  Serial.printf("  Size        %.2fMb\n", ESP.getFlashChipSize()/1000000.0);
  Serial.printf("  Speed       %.2fMHz\n", ESP.getFlashChipSpeed()/1000000.0);
  Serial.printf("  Mode        %d\n", ESP.getFlashChipMode());
  Serial.printf("  Cycles      %d\n", ESP.getCycleCount());
  Serial.printf("Chip\n");
  Serial.printf("  Cpu Freq    %dMHz\n", ESP.getCpuFreqMHz());
  Serial.printf("  Cores       %d\n", ESP.getChipCores());
  Serial.printf("  Model       %s\n", ESP.getChipModel());
  Serial.printf("  Revision    %d\n", ESP.getChipRevision());
  Serial.printf("  SDK Version %s\n", ESP.getSdkVersion());
}

void BlinkBlue()
{
digitalWrite(LED_BLUE, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(200);                       // wait for a second
  digitalWrite(LED_BLUE, LOW);   // turn the LED off by making the voltage LOW
  delay(100);                      // wait for a second
  
}

// the loop function runs over and over again forever
void loop() {
  _loop += 5;
  Serial.printf("Line %d\r\n", _loop);

  digitalWrite(LED_GREEN, HIGH);  // turn the LED on (HIGH is the voltage level)
  BlinkBlue();
  BlinkBlue();
  BlinkBlue();
  digitalWrite(LED_GREEN, LOW);  // turn the LED on (HIGH is the voltage level)
  
  BlinkBlue();
  BlinkBlue();
  BlinkBlue();
}
