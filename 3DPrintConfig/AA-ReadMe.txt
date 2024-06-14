Browser the PI using Putty
Host     pi@octopi.local
Port     22
Password C...12

On PC Copy current config
	cd C:\Program Files\PuTTY
	
	
	Read from Rasberry PI (Safe)
		pscp pi@octopi.local:printer.cfg C:\Code\Arduino\3DPrintConfig\printer-2024MMDD.cfg

	WRITE TO Rasberry PI (Danger)
		pscp C:\Code\Arduino\3DPrintConfig\printer.cfg pi@octopi.local:printer.cfg

TODO :
	Add FIRMWARE_RESTART between printes that involve filement change
	
	FIRMWARE_RESTART
	Restart with SWAP_RESUME
	