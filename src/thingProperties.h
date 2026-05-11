#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[] = "b7e37a48-2fbf-4683-97c0-ed7e66895ca3";
const char SSID[] = SECRET_SSID;
const char PASS[] = SECRET_OPTIONAL_PASS;
const char DEVICE_KEY[] = SECRET_DEVICE_KEY;

bool inverterOn;
float batteryPct;
bool gridAvailable;

void initProperties()
{

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(inverterOn, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(batteryPct, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(gridAvailable, READ, ON_CHANGE, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);