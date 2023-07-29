#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wregister"


#include "config.h"
#include "networking.h"
#include "util.h"
#include "lights.h"
#include "http.h"



void initAll() {
	// setup filesystem
	initFS();

	// init the logs
	initLogs();

	// setup the config values
	initConfig();

	initNetworking();

	getHttpServer()->init();
	// fire up the LEDs
  //delay(5000);
	initLEDs();
	// pause for things to catch up
	delay(500);
}




void setup() {
	//Serial.begin(9600);
	initAll();
}


void loop() {
	networkingLoop();
	getHttpServer()->loop();
	getLEDController()->loop();
}

