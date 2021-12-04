### Branches:
- **Tijn_FileSystem_API:** This branch contains all the file system API functions. This API allows easy interaction with the file system.
- **JonasLCD_functions:** This branch contains all the LCD functions. This API allows easy interaction with the LCD.
- **rik:** This branch contains all the HTTP functions. This allows for easy interaction with the webinterface.
- **Stijn_TCP:** This branch contains the all TCP functions. This allows for easy interaction with the tcp server.
- **tim-udp:** This branch contains all the UDP functions. Using these UDP functions, the IP can easily be acquired
- **MQTT:** This branch contains all the MQTT functions. This allows for easy interaction using the MQTT protocol.


#### FileSystem API notes:
-**QSPI**  
	QSPI is used as storage for all the files of the file system. 
	The ([following steps (step 6)](https://cdn.discordapp.com/attachments/901097670859452501/908738319968530482/De_QSPI_Flash_chip_gebruiken_op_het_discovery_platform.pdf) are required when the content of fsdata_custom.c doesn't match with the data on the QSPI chip.
	After programming, the setting can be turned back off.
	
-**Image syntax**  
	The .raw files use a special syntax in the name, so the API can extract the frame number, width, heigth and frame time out of the file name.
	The syntax is: `/Folder/...../Name#NUMBER#WIDTHxHEIGTH@TIME.ext`
	