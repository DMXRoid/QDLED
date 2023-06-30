#include <ArduinoJson.h>

#define FASTLED_ALLOW_INTERRUPTS 0
#define LISTEN_PORT 6969
#define LEASE_DURATION 3600

#define CONFIG_MDNS "mdns_name"
#define CONFIG_FRIENDLY "friendly_name"
#define CONFIG_COLOR "color"
#define CONFIG_WIFI "wifi"

#define LIGHT_MODE_SINGLE_COLOR 0
#define LIGHT_MODE_MULTI_COLOR_STATIC 1
#define LIGHT_MODE_MULTI_COLOR_STEP 2
#define LIGHT_MODE_GRADIENT_FADE 3

#define WIFI_MODE_UNCONFIGURED 0
#define WIFI_MODE_STATION 1
#define WIFI_MODE_SOFT_AP 2
#define WIFI_MODE_DUAL 3

#define WIFI_SSID "bearony"
#define WIFI_PASSWORD "bloodyshits"


#define LOG_FILE "/logs.txt"
#define CONFIG_FILE "/config.json"


extern StaticJsonDocument<512> config;




void initConfig();
void checkIsConfigured();
void saveConfig();