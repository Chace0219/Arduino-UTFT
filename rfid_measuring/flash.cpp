
#include "flash.h"

int timeOut = 2000;          //TimeOut is 2 seconds. This is the amount of time you wish to wait for a response from the CH376S module.
String wrData = "What is the meaning of life ?";     //We will write this data to a newly created file.
String wrData2 = "42";                                   //We will append this data to an already existing file.
uint8_t USB_Byte;               //used to store data coming from the USB stick

// Init Serial Routine
void initFlash()
{
	USB.begin(9600);
}

// CheckUSB Status,
uint8_t checkUSB()
{
	if (USB.available()) {                                 // This is here to capture any unexpected data transmitted by the CH376S module
		Serial.print(F("CH376S has just sent this code:"));
		Serial.println(USB.read(), HEX);
	}
	return 0;
}

//checkConnection==================================================================================
//This function is used to check for successful communication with the CH376S module. This is not dependant of the presence of a USB stick.
//Send any value between 0 to 255, and the CH376S module will return a number = 255 - value. 
bool checkConnection(byte value) {
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x06);
	USB.write(value);

	if (waitForResponse("checking connection")) {       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
		if (getResponseFromUSB() == (255 - value)) {
			Serial.println(F(">Connection to CH376S was successful."));
			//blinkLED();                               //blink the LED for 1 second if the connection was successful
			return true;
		}
		else {
			Serial.print(F(">Connection to CH376S - FAILED."));
			return false;
		}
	}
}

//set_USB_Mode=====================================================================================
//Make sure that the USB is inserted when using 0x06 as the value in this specific code sequence
void set_USB_Mode(byte value) {
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x15);
	USB.write(value);

	delay(20);

	if (USB.available()) {
		USB_Byte = USB.read();
		//Check to see if the command has been successfully transmitted and acknowledged.
		if (USB_Byte == 0x51) {                                   // If true - the CH376S has acknowledged the command.
			Serial.println(F("set_USB_Mode command acknowledged")); //The CH376S will now check and monitor the USB port
			USB_Byte = USB.read();

			//Check to see if the USB stick is connected or not.
			if (USB_Byte == 0x15) {                               // If true - there is a USB stick connected
				Serial.println(F("USB is present"));
				//blinkLED();                                     // If the process was successful, then turn the LED on for 1 second 
			}
			else {
				Serial.print(F("USB Not present. Error code:"));   // If the USB is not connected - it should return an Error code = FFH
				Serial.print(USB_Byte, HEX);
				Serial.println(F("H"));
			}

		}
		else {
			Serial.print(F("CH3765 error!   Error code:"));
			Serial.print(USB_Byte, HEX);
			Serial.println(F("H"));
		}
	}
	delay(20);
}

//resetALL=========================================================================================
//This will perform a hardware reset of the CH376S module - which usually takes about 35 msecs =====
void resetALL() {
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x05);
	Serial.println(F("The CH376S module has been reset !"));
	delay(200);
}

//readFile=====================================================================================
//This will send a series of commands to read data from a specific file (defined by fileName)
void readFile(String fileName) {
	resetALL();                     //Reset the module
	set_USB_Mode(0x06);             //Set to USB Mode
	diskConnectionStatus();         //Check that communication with the USB device is possible
	USBdiskMount();                 //Prepare the USB for reading/writing - you need to mount the USB disk for proper read/write operations.
	setFileName(fileName);          //Set File name
	fileOpen();                     //Open the file for reading
	int fs = getFileSize();         //Get the size of the file
	fileRead();                     //***** Send the command to read the file ***
	fileClose(0x00);                //Close the file
}

//writeFile========================================================================================
//is used to create a new file and then write data to that file. "fileName" is a variable used to hold the name of the file (e.g TEST.TXT). "data" should not be greater than 255 bytes long. 
void writeFile(String fileName, String data) {
	resetALL();                     //Reset the module
	set_USB_Mode(0x06);             //Set to USB Mode
	diskConnectionStatus();         //Check that communication with the USB device is possible
	USBdiskMount();                 //Prepare the USB for reading/writing - you need to mount the USB disk for proper read/write operations.
	setFileName(fileName);          //Set File name
	if (fileCreate())
	{               //Try to create a new file. If file creation is successful
		fileWrite(data);              //write data to the file.
	}

	//delay(1000);
	/*
	else {
	Serial.println("File could not be created, or it already exists");
	}*/
	fileClose(0x01);
}

//appendFile()====================================================================================
//is used to write data to the end of the file, without erasing the contents of the file.
bool appendFile(String fileName, String data) {
	bool result = false;
	resetALL();                     //Reset the module
	set_USB_Mode(0x06);             //Set to USB Mode
	diskConnectionStatus();         //Check that communication with the USB device is possible
	if (!USBdiskMount())
	{
		Serial.println();
		return false;
	}
		//Prepare the USB for reading/writing - you need to mount the USB disk for proper read/write operations.
	setFileName(fileName);          //Set File name
	fileOpen();                     //Open the file
	filePointer(false);             //filePointer(false) is to set the pointer at the end of the file.  filePointer(true) will set the pointer to the beginning.
	fileWrite(data);                //Write data to the end of the file
	if (fileClose(0x01))
		return true;
		//Close the file using 0x01 - which means to update the size of the file on close.
	return false;
}

//setFileName======================================================================================
//This sets the name of the file to work with
void setFileName(String fileName) {
	Serial.print(F("Setting filename to:"));
	Serial.println(fileName);
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x2F);
	USB.write(0x2F);         // Every filename must have this byte to indicate the start of the file name.
	USB.print(fileName);     // "fileName" is a variable that holds the name of the file.  eg. TEST.TXT
	USB.write((byte)0x00);   // you need to cast as a byte - otherwise it will not compile.  The null byte indicates the end of the file name.
	delay(20);
}

//diskConnectionStatus================================================================================
//Check the disk connection status
void diskConnectionStatus() {
	Serial.println(F("Checking USB disk connection status"));
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x30);

	if (waitForResponse("Connecting to USB disk")) {       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
		if (getResponseFromUSB() == 0x14) {               //CH376S will send 0x14 if this command was successful
			Serial.println(F(">Connection to USB OK"));
		}
		else {
			Serial.print(F(">Connection to USB - FAILED."));
		}
	}
}

//USBdiskMount========================================================================================
//initialise the USB disk and check that it is ready - this process is required if you want to find the manufacturing information of the USB disk
bool USBdiskMount() {
	Serial.println(F("Mounting USB disk"));
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x31);

	if (waitForResponse(F("mounting USB disk"))) {       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
		if (getResponseFromUSB() == 0x14) {               //CH376S will send 0x14 if this command was successful
			Serial.println(F(">USB Mounted - OK"));
			return true;
		}
		else {
			Serial.print(F(">Failed to Mount USB disk."));
		}
	}
	return false;
}

//fileOpen========================================================================================
//opens the file for reading or writing
void fileOpen() {
	Serial.println(F("Opening file."));
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x32);
	if (waitForResponse("file Open")) {                 //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
		if (getResponseFromUSB() == 0x14) {                 //CH376S will send 0x14 if this command was successful  
			Serial.println(F(">File opened successfully."));
		}
		else {
			Serial.print(F(">Failed to open file."));
		}
	}
}

//setByteRead=====================================================================================
//This function is required if you want to read data from the file. 
bool setByteRead(byte numBytes) {
	bool bytesToRead = false;
	int timeCounter = 0;
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x3A);
	USB.write((byte)numBytes);   //tells the CH376S how many bytes to read at a time
	USB.write((byte)0x00);
	if (waitForResponse("setByteRead")) {       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
		if (getResponseFromUSB() == 0x1D) {         //read the CH376S message. If equal to 0x1D, data is present, so return true. Will return 0x14 if no data is present.
			bytesToRead = true;
		}
	}
	return(bytesToRead);
}

//getFileSize()===================================================================================
//writes the file size to the serial Monitor.
int getFileSize() {
	int fileSize = 0;
	Serial.println(F("Getting File Size"));
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x0C);
	USB.write(0x68);
	delay(100);
	Serial.print(F("FileSize ="));
	if (USB.available()) {
		fileSize = fileSize + USB.read();
	}
	if (USB.available()) {
		fileSize = fileSize + (USB.read() * 255);
	}
	if (USB.available()) {
		fileSize = fileSize + (USB.read() * 255 * 255);
	}
	if (USB.available()) {
		fileSize = fileSize + (USB.read() * 255 * 255 * 255);
	}
	Serial.println(fileSize);
	delay(10);
	return(fileSize);
}


//fileRead========================================================================================
//read the contents of the file
void fileRead() {
	Serial.println(F("Reading file:"));
	byte firstByte = 0x00;                     //Variable to hold the firstByte from every transmission.  Can be used as a checkSum if required.
	byte numBytes = 0x40;                      //The maximum value is 0x40  =  64 bytes

	while (setByteRead(numBytes)) {              //This tells the CH376S module how many bytes to read on the next reading step. In this example, we will read 0x10 bytes at a time. Returns true if there are bytes to read, false if there are no more bytes to read.
		USB.write(0x57);
		USB.write(0xAB);
		USB.write(0x27);                          //Command to read ALL of the bytes (allocated by setByteRead(x))
		if (waitForResponse(F("reading data"))) {      //Wait for the CH376S module to return data. TimeOut will return false. If data is being transmitted, it will return true.
			firstByte = USB.read();                 //Read the first byte
			while (USB.available()) {
				Serial.write(USB.read());           //Send the data from the USB disk to the Serial monitor
				delay(1);                           //This delay is necessary for successful Serial transmission
			}
		}
		if (!continueRead()) {                       //prepares the module for further reading. If false, stop reading.
			break;                                   //You need the continueRead() method if the data to be read from the USB device is greater than numBytes.
		}
	}
	Serial.println();
	Serial.println(F("NO MORE DATA"));
}

//fileWrite=======================================================================================
//are the commands used to write to the file
void fileWrite(String data) {
	Serial.println(F("Writing to file:"));
	byte dataLength = (byte)data.length();         // This variable holds the length of the data to be written (in bytes)
	Serial.println(data);
	Serial.print(F("Data Length:"));
	Serial.println(dataLength);
	delay(100);
	// This set of commands tells the CH376S module how many bytes to expect from the Arduino.  (defined by the "dataLength" variable)
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x3C);
	USB.write((byte)dataLength);
	USB.write((byte)0x00);
	if (waitForResponse("setting data Length")) {      // Wait for an acknowledgement from the CH376S module before trying to send data to it
		if (getResponseFromUSB() == 0x1E) {                // 0x1E indicates that the USB device is in write mode.
			USB.write(0x57);
			USB.write(0xAB);
			USB.write(0x2D);
			USB.print(data);                             // write the data to the file

			if (waitForResponse("writing data to file")) {   // wait for an acknowledgement from the CH376S module
			}
			Serial.print(F("Write code (normally FF and 14): "));
			Serial.print(USB.read(), HEX);                // code is normally 0xFF
			Serial.print(F(","));
			USB.write(0x57);
			USB.write(0xAB);
			USB.write(0x3D);                             // This is used to update the file size. Not sure if this is necessary for successful writing.
			if (waitForResponse("updating file size")) {   // wait for an acknowledgement from the CH376S module
			}
			Serial.println(USB.read(), HEX);              //code is normally 0x14*/
		}
	}
}

//continueRead()==================================================================================
//continue to read the file : I could not get this function to work as intended.
bool continueRead() {
	bool readAgain = false;
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x3B);
	if (waitForResponse("continueRead")) {       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
		if (getResponseFromUSB() == 0x14) {         //CH376S will send 0x14 if this command was successful
			readAgain = true;
		}
	}
	return(readAgain);
}

//fileCreate()========================================================================================
//the command sequence to create a file
bool fileCreate() {
	bool createdFile = false;
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x34);
	if (waitForResponse("creating file")) {       //wait for a response from the CH376S. If file has been created successfully, it will return true.
		if (getResponseFromUSB() == 0x14) {          //CH376S will send 0x14 if this command was successful
			createdFile = true;
		}
	}
	return(createdFile);
}


//fileDelete()========================================================================================
//the command sequence to delete a file
void fileDelete(String fileName) {
	setFileName(fileName);
	delay(20);
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x35);
	if (waitForResponse("deleting file")) {       //wait for a response from the CH376S. If file has been created successfully, it will return true.
		if (getResponseFromUSB() == 0x14) {          //CH376S will send 0x14 if this command was successful
			Serial.println("Successfully deleted file");
		}
	}
}


//filePointer========================================================================================
//is used to set the file pointer position. true for beginning of file, false for the end of the file.
void filePointer(bool fileBeginning) {
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x39);
	if (fileBeginning) {
		USB.write((byte)0x00);             //beginning of file
		USB.write((byte)0x00);
		USB.write((byte)0x00);
		USB.write((byte)0x00);
	}
	else {
		USB.write((byte)0xFF);             //end of file
		USB.write((byte)0xFF);
		USB.write((byte)0xFF);
		USB.write((byte)0xFF);
	}
	if (waitForResponse("setting file pointer")) {       //wait for a response from the CH376S. 
		if (getResponseFromUSB() == 0x14) {                 //CH376S will send 0x14 if this command was successful
			Serial.println("Pointer successfully applied");
		}
	}
}


//fileClose=======================================================================================
//closes the file
bool fileClose(uint8_t closeCmd) {
	Serial.println(F("Closing file:"));
	USB.write(0x57);
	USB.write(0xAB);
	USB.write(0x36);
	USB.write((byte)closeCmd);                                // closeCmd = 0x00 = close without updating file Size, 0x01 = close and update file Size

	if (waitForResponse("closing file")) {                      // wait for a response from the CH376S. 
		byte resp = getResponseFromUSB();
		if (resp == 0x14) {                                        // CH376S will send 0x14 if this command was successful
			Serial.println(F(">File closed successfully."));
			return true;
		}
		else {
			Serial.print(F(">Failed to close file. Error code:"));
			Serial.println(resp, HEX);
		}
	}
	return false;
}

//waitForResponse===================================================================================
//is used to wait for a response from USB. Returns true when bytes become available, false if it times out.
bool waitForResponse(String errorMsg) {
	boolean bytesAvailable = true;
	int counter = 0;
	while (!USB.available()) {     //wait for CH376S to verify command
		delay(1);
		counter++;
		if (counter>timeOut) {
			Serial.print(F("TimeOut waiting for response: Error while: "));
			Serial.println(errorMsg);
			bytesAvailable = false;
			break;
		}
	}
	delay(1);
	return(bytesAvailable);
}

//getResponseFromUSB================================================================================
//is used to get any error codes or messages from the CH376S module (in response to certain commands)
uint8_t getResponseFromUSB() {
	uint8_t response = 0x00;
	if (USB.available()) {
		response = USB.read();
	}
	return(response);
}
