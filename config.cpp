#include <FS.h>
#include <ArduinoJson.h>

#include "config.h"
#include "util.h"

String initialConfig = R"(
	{
		"color": {
			"mode": 0,
			"fade_delay": 50,
			"step_delay": 500,
			"is_gradient": false,
			"colors": [
				"0xFF0000",
				"0xFFFFFF",
				"0x0000FF"
			]
		},
		"lights": {
			"count": 20,
			"start_time": 0,
			"end_time": 0,
			"is_enabled": true,
			"brightness": 255
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
		"ip_address": "0.0.0.0",
		"mac_address": ""

	}
)";







bool isConfigured;

StaticJsonDocument<2048> config;

void initConfig() {
  File configFile = SPIFFS.open(CONFIG_FILE, "r");
  String cf = configFile.readString();
  configFile.close();
  String c = (cf != "") ? cf : initialConfig;
  log("c string:::" + c);
  DeserializationError e = deserializeJson(config, c);
  if(e) {
	log("Deserialization error: " + String(e.f_str()));
  }
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
	File configFile = SPIFFS.open(CONFIG_FILE, "w");
	serializeJson(config, configFile);
	configFile.close();
}
