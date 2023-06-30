

#include "config.h"
#include "util.h"
#include "networking.h"


void initNetworking() {
	initWifi();
	refreshmDNS();
	initOTA();
}


void refreshmDNS() {
	setmDNSName();
	log("Refreshing mDNS setup for " + config[CONFIG_MDNS].as<String>() + "..."); 
	if(!MDNS.begin(config[CONFIG_MDNS].as<String>())) {
		log("mDNS setup failed");
	}
	else {
		MDNS.addService("http","tcp", LISTEN_PORT);
		log("mDNS setup complete!");
	}
}

void setmDNSName() {
	String n;
	n = config[CONFIG_MDNS].as<String>();
	log("first ln: " + n);
	if (n == "") {
		n = "new-light-" + String(int(floor(random(1,100))));
		log(n);
	}
	config[CONFIG_MDNS] = n;
}

void initWifi() {

	// defines will override config values if there are no config values 

	if(
		WIFI_SSID != "" 
		&& WIFI_PASSWORD != "" 
		&& config["wifi"]["ssid"] == "" 
		&& config["wifi"]["password"] == ""
		) {
		config["wifi"]["ssid"] = WIFI_SSID;
		config["wifi"]["password"] = WIFI_SSID;
		config["wifi"]["mode"] = WIFI_MODE_STATION;
	}

	switch(int(config["wifi"]["mode"])) {
		case WIFI_MODE_UNCONFIGURED:
			break;

		case WIFI_MODE_STATION:
			initStationWifi();
			break;

		case WIFI_MODE_SOFT_AP:
			break;

		case WIFI_MODE_DUAL:
			break;
	}

}


void initStationWifi() {
	WiFi.begin(String(config[CONFIG_WIFI]["ssid"]), String(config[CONFIG_WIFI]["password"]));
	
	log("Connecting to " + String(config[CONFIG_WIFI]["ssid"]));
	int x = 0;

	while ( WiFi.status() != WL_CONNECTED) {

		delay(1000);
		log("Attempt #" + String(x));
		x++;
	}
	
	log("Connection established!");	
	log("IP Address: " + WiFi.localIP().toString());
}

void initOTA() {
	log("starting OTA init");
	ArduinoOTA.onStart([]() {
		log("Starting OTA");
	});
	ArduinoOTA.onEnd([]() {
		log("Ending OTA");
	});

	ArduinoOTA.onError([](ota_error_t error) {
		log("OTA Error: " + String(error));
	});

	ArduinoOTA.onProgress([](unsigned int p, unsigned int t) {
		log("OTA Progress: " + String((p / t) * 100));
	});

	ArduinoOTA.begin();
}



void networkingLoop() {
	ArduinoOTA.handle();
	MDNS.update();
}
