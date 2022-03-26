
//////////////////////////////////
//   DEVICE-SPECIFIC SERVICES   //
//////////////////////////////////

struct DEV_Identify : Service::AccessoryInformation
{

	int nBlinks;                    // number of times to blink built-in LED in identify routine
	SpanCharacteristic *identify;   // reference to the Identify Characteristic

	DEV_Identify(const char *name, const char *manu, const char *sn, const char *model, const char *version, int nBlinks) : Service::AccessoryInformation()
	{

		new Characteristic::Name(name);                   // create all the required Characteristics with values set based on above arguments
		new Characteristic::Manufacturer(manu);
		new Characteristic::SerialNumber(sn);
		new Characteristic::Model(model);
		new Characteristic::FirmwareRevision(version);
		identify=new Characteristic::Identify();          // store a reference to the Identify Characteristic for use below

		this->nBlinks=nBlinks;                            // store the number of times to blink the LED

		pinMode(homeSpan.getStatusPin(),OUTPUT);          // make sure LED is set for output

		// JRM:Toggle pin 10
		pinMode(10,OUTPUT);   
	}

	boolean update()
	{
		for(int i=0; i<nBlinks; i++)
		{
			digitalWrite(10, HIGH);    
			_neoPixel.Fill( 255,0, 0,100);
			delay(100);
			_neoPixel.Fill( 0, 255, 0,100);
			delay(100);
			_neoPixel.Fill( 0, 0, 255,100);
			delay(100);
			digitalWrite(10, LOW);    

			_neoPixel.Fill( 255,0, 255,100);
			delay(100);
			_neoPixel.Fill( 255,255, 0,100);
			delay(100);
		}
		_neoPixel.Off();
		return(true);                               // return true

	} // update

};
