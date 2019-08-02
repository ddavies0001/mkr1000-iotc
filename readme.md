# Arduino MKR1000/1010 with Azure IoT Central

## About

The original version of this fork showed how to exchange limited telemetry and properties, settings, etc between Arduino MKR1000 or MKR1010 devices and Azure IoT Central.  This fork illustrates more device types and operations.

## Features

* Works with both the Arduino MKR1000 or MKR1010 devices
* Illutrates use of other devices
* Uses a DHT11 or DHT22 sensor for temperature and humidity.  More sensors covered in this document.
* Uses simple MQTT library to communicate to Azure IoT Central
* Simple code base designed to illustrate how the code works and encourage hacking (~400 lines of core code w/ comments)
* Supports the use of Azure IoT Device Provisioning Service (DPS) for registering the device in IoT Central
* IoT Central features supported
  * Telemetry data - Temperature and Humidity now, more later
  * Properties - Device sends a die roll number every 15 seconds
  * Settings - Change the fan speed value and see it displayed in the serial moitor and acknowledged back to IoT Central
  * Commands - Send a message to the device and see it displayed as morse code on the device LED

## Installation

Run:

```
git clone https://github.com/ddavies0001/mkr1000-iotc.git
```

## Prerequisite


Install the Arduino IDE (I tested with the current latest version 1.8.9) and the necessary drivers for the Arduino MKR1000 series of boards and ensure that a simple LED blink sketch compiles and runs on the board.  Follow the guide here https://www.arduino.cc/en/Guide/MKR1000

If you have been exploring various ways to connect to the cloud from Arduino (AWS with no encryption, AWS with encryption, Google Cloud, Microsoft Azure IoT Hub, Microsoft Azure IoT Central, etc), you may find it useful to separate the Arduino libraries discussed in the next few paragraphs so that each cloud connection method has its own set of libraries.  You can do this by picking a directory for each set of libraries and use the Arduino IDE to specify which directory to use.  In my case, I installed the cloud-specific diretories just below %HOMEPATH%/Documents/Arduino and above "libraries".  For example, %HOMEPATH%/Documents/Arduino/iotc/libraries would have the libraries for the Azure IoT Central libraries.  To specify which libraries are active, open the Arduino IDE, select File -> Preferences and put the directory path (e.g %HOMEPATH%/Documents/Arduino/iotc) into the "Sketchbook location".

This code requires a couple of libraries to be installed for it to compile.  Depending on if you are using a MKR1000 or MKR1010 board the Wi-Fi libraries are different.  To install an Arduino library, open the Arduino IDE and click the "Sketch" menu and then "Include Library" -> "Manage Libraries".  In the dialog, filter by the library name below and install the latest version.  For more information on installing libraries with Arduino see https://www.arduino.cc/en/guide/libraries. 

### MKR1000:

* Install library "Wifi101"
* Install library "SimpleDHT"
* Install library "RTCZero"
* Install library "PubSubClient"

### MKR1010:

* Install library "WiFiNINA"
* Install library "SimpleDHT"
* Install library "RTCZero"
* Install library "PubSubClient"

**Note** - We need to increase the payload size limit in PubSubClient to allow for the larger size of MQTT messages from the Azure IoT Hub.  Open the file at %HomePath%\Documents\Arduino\<library selector directory>\libraries\PubSubClient\src\PubSubClient.h in your favorite code editor.  Change the line (line 26 in current version):

``` C
#define MQTT_MAX_PACKET_SIZE 128
```

to:

``` C
#define MQTT_MAX_PACKET_SIZE 2048
```

Save the file and you have made the necessary fix.  The size probably does not need to be this large but firedog1024 did not find the crossover point where the size causes a failure.  Fortunately the MKR1000/1010 has a pretty good amount of SRAM (32KB!) so we should be ok.

To connect the device to Azure IoT Central you will need to provision an IoT Central application.  This is free for **seven days** but if you already have signed up for an Azure subscription and want to use pay as you go IoT Central is free as long as you have no more than **five devices** and do not exceed **1MB per month** of data.  The Free Trial subscription won't let you have more than one IoT Central application at a time.  That's what convinced me to convert to a pay-as-you-go subscription.

Go to https://apps.azureiotcentral.com/ to create an application (you will need to sign in with a Microsoft account identity you may already have one if you use Xbox, office365, Windows 10, or other Microsoft services).  Beware, confusion ahead!  If you go to https://apps.azureiotcentral.com/, you get a nice way to make IoT Central applications.  Say you close your browser and go out for lunch.  Sometime later, you open the browser and want to find your application again. You might reasonably go to the Azure Portal and search for something like "Azure IoT Central applications."  That will take you to a page labelled "IoT Central Application"  Sadly, that page will say that there are no IoT Central applications.  I don't know a way around this other than sticking to https://apps.azureiotcentral.com/.

* Choose Trial or Pay-As-You-Go.
* Select the Sample DevKits template (middle box)
* Provide an application name and URL domain name 
* If you select Pay-As-You-Go you will need to select your Azure subscription and select a region to install the application into.  This information is not needed for Trial.
* Click "Create"

You should now have an IoT Central application provisioned so lets add a real device.  Click Devices on the left.  You will now see three templates in the left hand panel (MXChip, Raspberry Pi, Windows 10 IoT Core).  We are going to use the MXChip template for this exercise to avoid having to create a new template.  Click "MXChip" and click the "+V" icon on the toolbar, this will present a drop down where we click "Real" to add a new physical device.  Give a name to your device and click "Create".  

You now have a device in IoT Central that can be connected to from the Arduino MKR1000/1010 device.  Proceed to wiring and configuration.

## Wiring

![wiring diagram for mkr1000/1010 and DHT11/22](https://github.com/firedog1024/mkr1000-iotc/raw/master/assets/mkr1000_dht.png)

From left to right pins on the DHT11/22 sensor:

1. VCC (voltage 3.3v or 5v, MKR uses 3.3V natively)
2. ~2 (data)
3. Not connected
4. GND (ground)

## Configuration

We need to copy some values from our new IoT Central device into the configure.h file so it can connect to IoT Central.

Click the device you created at the end of the Prerequisite.  In the upper right corner of the device's window are three links: "Block", "Connect" and "Delete".  Click the "Connect" link to get the connection information for the device.  We are going to copy "Scope ID', Device ID", and "Primary Key" values into the respective positions in the configure.h file.

``` C
// Azure IoT Central device information
static char PROGMEM iotc_scopeId[] = "<replace with IoT Central scope-id>";
static char PROGMEM iotc_deviceId[] = "<replace with IoT Central device id>";
static char PROGMEM iotc_deviceKey[] = "<replace with IoT Central device key>";
```

You will also need to provide the Wi-Fi SSID (Wi-Fi name) and password in the configure.h

``` C
// Wi-Fi information
static char PROGMEM wifi_ssid[] = "<replace with Wi-Fi SSID>";
static char PROGMEM wifi_password[] = "<replace with Wi-Fi password>";
```

Finally we need to tell the code what DHT sensor we are using.  This can be the DHT22 (white), DHT11 (blue), or none and have the code simulate the values.  Comment and uncomment the appropriate lines in configure.h

``` C
// comment / un-comment the correct sensor type being used
//#define SIMULATE_DHT_TYPE
//#define DHT11_TYPE
#define DHT22_TYPE
```

If for any reason you use a different GPIO pin for the data you can also change the pin number in the configure.h file

``` C
// for DHT11/22, 
//   VCC: 5V or 3V
//   GND: GND
//   DATA: 2
int pinDHT = 2;
```

## Compiling and running

Now that you have configured the code with IoT Central, Wi-Fi, and DHT sensor information we are ready to compile and run the code on the device.

Load the mkr10x0_iotc\mkr10x0_iotc.ino file into the Arduino IDE and click the Upload button on the toolbar.  The code should compile and be uploaded to the device.  In the output window you should see:


```
Sketch uses 71816 bytes (27%) of program storage space. Maximum is 262144 bytes.
Atmel SMART device 0x10010005 found
Device       : ATSAMD21G18A
Chip ID      : 10010005
Version      : v2.0 [Arduino:XYZ] Dec 20 2016 15:36:43
Address      : 8192
Pages        : 3968
Page Size    : 64 bytes
Total Size   : 248KB
Planes       : 1
Lock Regions : 16
Locked       : none
Security     : false
Boot Flash   : true
BOD          : true
BOR          : true
Arduino      : FAST_CHIP_ERASE
Arduino      : FAST_MULTI_PAGE_WRITE
Arduino      : CAN_CHECKSUM_MEMORY_BUFFER
Erase flash
done in 0.846 seconds

Write 72472 bytes to flash (1133 pages)

[=                             ] 5% (64/1133 pages)
[===                           ] 11% (128/1133 pages)
[=====                         ] 16% (192/1133 pages)
[======                        ] 22% (256/1133 pages)
[========                      ] 28% (320/1133 pages)
[==========                    ] 33% (384/1133 pages)
[===========                   ] 39% (448/1133 pages)
[=============                 ] 45% (512/1133 pages)
[===============               ] 50% (576/1133 pages)
[================              ] 56% (640/1133 pages)
[==================            ] 62% (704/1133 pages)
[====================          ] 67% (768/1133 pages)
[======================        ] 73% (832/1133 pages)
[=======================       ] 79% (896/1133 pages)
[=========================     ] 84% (960/1133 pages)
[===========================   ] 90% (1024/1133 pages)
[============================  ] 96% (1088/1133 pages)
[==============================] 100% (1133/1133 pages)
done in 0.482 seconds

Verify 72472 bytes of flash with checksum.
Verify successful
done in 0.060 seconds
CPU reset.
```
The code is now running on the device and should be sending data to IoT Central.  We can look at the serial port monitor by clicking the Tool menu -> Serial Monitor (you may need to change the baud rate to 115200).  You should start to see output displayed in the window.  If you are using a MKR1000 and see the following messages output then there is an issue connecting to the IoT Hub via MQTT due to invalid certificates:

```
---> mqtt failed, rc=-2
```

To fix this we need to update the Wi-Fi firmware on the device to the latest version (19.6.1 for the MKR1000).  Follow the instructions here https://www.arduino.cc/en/Tutorial/FirmwareUpdater to update the firmware to the latest version (currently 19.6.1).  Then start this section from the beginning.

### Telemetry:
If the device is working correctly you should see output like this in the serial monitor that indicates data is successfully being transmitted to Azure IoT Central:

```
===> mqtt connected
Current state of device twin:
	{
  "desired": {
    "fanSpeed": {
      "value": 200
    },
    "$version": 16
  },
  "reported": {
    "dieNumber": 1,
    "fanSpeed": {
      "value": 200,
      "statusCode": 200,
      "status": "completed",
Sending telemetry ...
	{"temp":   25.30, "humidity":   27.70}
Sending telemetry ...
	{"temp":   25.10, "humidity":   27.10}
Sending digital twin property ...
Sending telemetry ...
	{"temp":   25.10, "humidity":   29.60}
--> IoT Hub acknowledges successful receipt of twin property: 1
Sending telemetry ...
	{"temp":   25.60, "humidity":   29.20}
Sending telemetry ...
	{"temp":   25.10, "humidity":   28.20}
Sending digital twin property ...
Sending telemetry ...
	{"temp":   25.20, "humidity":   28.50}
--> IoT Hub acknowledges successful receipt of twin property: 2
Sending telemetry ...
	{"temp":   25.30, "humidity":   28.40}
Sending telemetry ...
	{"temp":   25.30, "humidity":   27.70}
Sending digital twin property ...
Sending telemetry ...
	{"temp":   25.30, "humidity":   27.30}
--> IoT Hub acknowledges successful receipt of twin property: 3
Sending telemetry ...
	{"temp":   25.20, "humidity":   28.00}
```

Now that we have data being sent lets look at the data in our IoT Central application.  Click the device you created and then select the temperature and humidity telemetry values in the Telemetry column.  You can turn on and off telemetry values by clicking on the eyeballs.  We are only sending temperature and humidity so no other telemetry items will be active.  You should see a screen similar to this:

![telemetry screen shot](https://github.com/firedog1024/mkr1000-iotc/raw/master/assets/telemetry.png)

### Properties:
The device is also updating the property "Die Number", click on the "Properties" link at the top and you should see the value in the Die Number change about ever 15 seconds.

![properties screen shot](https://github.com/firedog1024/mkr1000-iotc/raw/master/assets/properties.png)

### Settings:
The device will accept settings and acknowledge the receipt of the setting back to IoT Central.  Go to the "Settings" link at the top and change the value for Fan Speed (RPM), then click the "Update" button the text below the input box will briefly turn red then go green when the device acknowledges receipt of the setting.  In the serial monitor the following should be observed:

```
Fan Speed setting change received with value: 200
{"fanSpeed":{"value":200,"statusCode":200,"status":"completed","desiredVersion":9}}
--> IoT Hub acknowledges successful receipt of twin property: 1
```

The settings screen should look something like this:

![settings screen shot](https://github.com/firedog1024/mkr1000-iotc/raw/master/assets/settings.png)

### Commands:
We can send a message to the device from IoT Central.  Go to the "Commands" link at the top and enter a message into the Echo - Value to display text box.  The message should consist of only alpha characters (a - z) and spaces, all other characters will be ignored.  Click the "Run" button and watch your device.  You should see the LED blink morse code.  If you enter SOS the led should blink back ...---... where dots are short blinks and dashes slightly longer :-)

![commands screen shot](https://github.com/firedog1024/mkr1000-iotc/raw/master/assets/commands.png)

The morse code blinking LED is here on the MKR1000

![morse code blinking LED location](https://github.com/firedog1024/mkr1000-iotc/raw/master/assets/blink_led.png)

## What Now?

You have the basics now go play and hack this code to send other sensor data to Azure IoT Central.  If you want to create a new device template for this you can learn how to do that with this documentation https://docs.microsoft.com/en-us/azure/iot-central/howto-set-up-template.

How about creating a rule to alert when the temperature or humidity exceed a certain value.  Learn about creating rules here https://docs.microsoft.com/en-us/azure/iot-central/tutorial-configure-rules.

For general documentation about Azure IoT Central you can go here https://docs.microsoft.com/en-us/azure/iot-central/.

Have fun!

