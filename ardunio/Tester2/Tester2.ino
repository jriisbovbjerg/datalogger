#include <OneWire.h>
#include <DallasTemperature.h>
#include <Console.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

unsigned long timer;
unsigned long counter = 0L;

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup(void)
{
  // initialize serial communication:
  Bridge.begin();
  Console.begin(); 

  while (!Console){
    ; // wait for Console port to connect.
  }
  Console.println("You're connected to the Console!!!!");

  timer = millis();
  
  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Console.print("Locating devices...");
  
  Console.print("Found ");
  Console.print(numberOfDevices, DEC);
  Console.println(" devices.");

  // report parasite power requirements
  Console.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Console.println("ON");
  else Console.println("OFF");
  
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Console.print("Found device ");
		Console.print(i, DEC);
		Console.print(" with address: ");
		printAddress(tempDeviceAddress);
		Console.println();
		
		Console.print("Setting resolution to ");
		Console.println(TEMPERATURE_PRECISION, DEC);
		
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		 Console.print("Resolution actually set to: ");
		Console.print(sensors.getResolution(tempDeviceAddress), DEC); 
		Console.println();
	}else{
		Console.print("Found ghost device at ");
		Console.print(i, DEC);
		Console.print(" but could not detect address. Check power and cabling");
	}
  }

}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Console.print(tempC);
}

void loop(void)
{ 
  /* Every 200ms: */

    if (millis() - timer > 2000) {
        timer = millis();

        Bridge.put("counter", String(counter++));
        Bridge.put("random1", String(random(1, 100)));

        // call sensors.requestTemperatures() to issue a global temperature 
        // request to all devices on the bus
        sensors.requestTemperatures(); // Send the command to get temperatures
  
        // Loop through each device, print out temperature data
        for(int i=0;i<numberOfDevices; i++)
        {
          // Search the wire for address
          if(sensors.getAddress(tempDeviceAddress, i))
      	{
      		// Output the device ID
      		// It responds almost immediately. Let's print out the data
          printAddress(tempDeviceAddress);
          Console.print(";");
      		printTemperature(tempDeviceAddress); // Use a simple function to print out the data
          Console.println("");
      	} 
      	//else ghost device! Check your power requirements and cabling
      	
        }
    }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Console.print("0");
    Console.print(deviceAddress[i], HEX);
  }
}
