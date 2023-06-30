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

		void setSingleColorAction(String);
		void setMultiColorStaticAction(JsonArray);
		void setMultiColorStepAction(JsonArray);
		void setGradientFadeAction(JsonArray);
		void setLightCountAction(int);
		
};


HttpServer* getHttpServer();
extern HttpServer* httpServer;

