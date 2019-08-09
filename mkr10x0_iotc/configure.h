// Azure IoT Central device information
static char PROGMEM iotc_scopeId[] = "0ne0006FE39";
static char PROGMEM iotc_deviceId[] = "b8f7905c-56a6-4002-9aa6-5e95a0d27d78";
static char PROGMEM iotc_deviceKey[] = "/Cia3uWro2wVBVlNkoN0G+lZyZjUrTV1ngaLGe5T6J0=";
static char PROGMEM iotc_connectionString[] = " HostName=iotc-a8a4e3ac-6d77-42de-acd6-3c69a161df36.azure-devices.net;DeviceId=b8f7905c-56a6-4002-9aa6-5e95a0d27d78;SharedAccessKey=/Cia3uWro2wVBVlNkoN0G+lZyZjUrTV1ngaLGe5T6J0=";

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
