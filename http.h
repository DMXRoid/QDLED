#include "ArduinoJson/Array/JsonArray.hpp"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


class HttpServer {
	public:
		HttpServer();
		void loop();
		void init();
	private:
		ESP8266WebServer* server;
		StaticJsonDocument<1024> responseMessage;
		StaticJsonDocument<1024> requestJSON;
		
		
		void sendResponse(String, int);
		void sendHTMLResponse(String, int);
		void sendJSONResponse();

		void handleJSONAction();		

		void setLightCountAction(int);
		void setColorsAction(JsonArray, bool);
		void setModeAction(int);
		void setBrightnessAction(int);
		void setIsEnabledAction(bool);
};


HttpServer* getHttpServer();
extern HttpServer* httpServer;

