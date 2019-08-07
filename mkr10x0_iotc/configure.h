// Azure IoT Central device information
static char PROGMEM iotc_scopeId[] = "0ne0006ECD0";
static char PROGMEM iotc_deviceId[] = "gigabits-device-001";
static char PROGMEM iotc_deviceKey[] = "OA/xtvsMYMNhzcRSl/HyLkQB1dSk5xqQT6DkPf3uwK0=";
static char PROGMEM iotc_connectionString[] = " HostName=iotc-69400b47-8251-4eb6-abfa-305ad5647ae8.azure-devices.net;DeviceId=gigabits-device-001;SharedAccessKey=OA/xtvsMYMNhzcRSl/HyLkQB1dSk5xqQT6DkPf3uwK0=";

// Wi-Fi information
static char PROGMEM wifi_ssid[] = "Bark Place";
static char PROGMEM wifi_password[] = "31f28065b6c93ee750b211de84";

// comment / un-comment the correct sensor type being used
#define SIMULATE_DHT_TYPE
#define DHT11_TYPE
//#define DHT22_TYPE

// for DHT11/22, 
//   VCC: 5V or 3V
//   GND: GND
//   DATA: 2
// int pinDHT = 2;
int pinDHT = A6;
