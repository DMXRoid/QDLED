#include <FS.h>
#include <ArduinoJson.h>

#include "config.h"
#include "util.h"

String initialConfig = R"(
	{
		"color": {
			"mode": 1,
			colors: [
				"0xFF0000",
				"0xFFFFFF",
				"0x0000FF"
			]
		},
		"lights": {
			"count": 18
		},
		"wifi": {
			"mode": 1,
			"ssid": "",
			"password": "",
			"pk": ""
		},
		"data_pin": 1,
		"mdns_name": "",
		"friendly_name": ""
	}
)";







bool isConfigured;

StaticJsonDocument<512> config;

void initConfig() {
  File configFile = LittleFS.open(CONFIG_FILE, "r");
  String cf = configFile.readString();
  configFile.close();
  String c = (cf != "") ? cf : initialConfig;
  log("c string:::" + c);
  deserializeJson(config, c);

  /*else {
		log("Initial config: " + initialConfig + " ... ");
		deserializeJson(config, initialConfig);
		serializeJson(config, configFile);
	}*/
}

void checkIsConfigured() {
  if (!isConfigured) {
	initConfig();
  }
}

void saveConfig() {
	File configFile = LittleFS.open(CONFIG_FILE, "w");
	serializeJson(config, configFile);
	configFile.close();
}
