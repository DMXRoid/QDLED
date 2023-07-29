#include "ArduinoJson/Array/JsonArray.hpp"
#include "config.h"
#include "util.h"
#include "networking.h"
#include "lights.h"
#include "http.h"

void(* resetFunc) (void) = 0;

HttpServer* httpServer;

HttpServer::HttpServer() {
	server = new ESP8266WebServer(80);
	log("Starting HTTP setup");
	// dump the logs 

}
void HttpServer::sendHTMLResponse(String rt, int code) {
	server->sendHeader("Cache-Control","nocache");
	server->send(code, "text/html; charset=utf-8", rt);
}

void HttpServer::sendJSONResponse() {
	server->sendHeader("Cache-Control","nocache");
	String rt;
	serializeJsonPretty(responseMessage, rt);
	server->send(responseMessage["code"], "application/json; charset=utf-8", rt);
}

void HttpServer::init() {

	server->on("/act", [this]() {
		requestJSON.clear();
		responseMessage.clear();
		deserializeJson(requestJSON, server->arg("plain"));
		
		handleJSONAction();
		sendJSONResponse();
	});

	server->on("/get-file",[this]() {
		String l = getFileContents(server->arg("n"));
		//log("requested file" + l);
		l.replace("\n","\n<br>\n");
		sendHTMLResponse(l, 200);
	});
	server->on("/ok",[this]() {
		sendHTMLResponse("ok", 200);
	});

		server->begin();
	log("HTTP setup complete");
}


void HttpServer::handleJSONAction() {
	if(requestJSON["action"]) {
		log("action: " + String(requestJSON["action"]));

		/*
			{
				"action": "get-config",
				"colors": [
					"0xFF0000",
					"0xFFFFFF",
					"0x0000FF"
				]
			}

		*/

		if(requestJSON["action"] == "get-config") {
			responseMessage["code"] = 200;
			responseMessage["message"] = config;
		}

		else if(requestJSON["action"] == "set-colors") {
			setColorsAction(requestJSON["colors"], requestJSON["is_gradient"]);
		}

		else if(requestJSON["action"] == "set-light-mode") {
			setModeAction(requestJSON["mode"]);
		}

		else if(requestJSON["action"] == "set-brightness") {
			setBrightnessAction(requestJSON["brightness"]);
		}
		else if(requestJSON["action"] == "set-is-enabled") {
			setIsEnabledAction(requestJSON["is_enabled"]);
		}
		else if(requestJSON["action"] == "set-data-pin") {
			config["data_pin"] = server->arg("p");
			saveConfig();
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}

		else if(requestJSON["action"] == "set-light-count") {
			setLightCountAction(requestJSON["count"].as<int>());
		}

		else if(requestJSON["action"] == "set-fade-delay") {
			log("setting fade delay to " + String(requestJSON["fade_delay"]));
			getLEDController()->setFadeDelay(requestJSON["fade_delay"]);
			config["color"]["fade_delay"] = requestJSON["fade_delay"];
			saveConfig();
		}
		else if(requestJSON["action"] == "set-step-delay") {
			log("setting step delay to " + String(requestJSON["step_delay"]));
			getLEDController()->setStepDelay(requestJSON["step_delay"]);
			config["color"]["step_delay"] = requestJSON["step_delay"];
			saveConfig();
		}
		else if(requestJSON["action"] == "set-light-count") {
			config["lights"]["count"] = requestJSON["count"];
			log("setting light count to " + String(requestJSON["count"]));
			setLightCountAction(requestJSON["count"].as<int>());
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}
		else if (requestJSON["action"] == "set-config") {
			config = requestJSON["config"];
			saveConfig();
			resetFunc();
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}

		else if (requestJSON["action"] == "set-color-config") {
			config["color"] = requestJSON["color-config"];
			saveConfig();
			getLEDController()->update();
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}

		else if (requestJSON["action"] == "set-light-config") {
			config["light"] = requestJSON["light-config"];
			saveConfig();
			getLEDController()->update();
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}


		/*
			{ "action" : "reset"}
		*/

		else if (requestJSON["action"] == "reset") {
			resetFunc();

		}
	}

}


void HttpServer::setLightCountAction(int newLightCount) {
	getLEDController()->setCount(newLightCount);
	getLEDController()->update();
	
	config["lights"]["count"] = newLightCount;
	saveConfig();

	responseMessage["code"] = 200;
	responseMessage["message"] = "success";
}

void HttpServer::setColorsAction(JsonArray c, bool isGradient) {
	getLEDController()->setColors(colorJSONToList(c));
	getLEDController()->setIsGradient(isGradient);
	getLEDController()->update();
	
	config["color"]["colors"] = c;
	config["color"]["is_gradient"] = isGradient;
	saveConfig();

	responseMessage["code"] = 200;
	responseMessage["message"] = "success";
}

void HttpServer::setModeAction(int m) {
	getLEDController()->setMode(m);
	getLEDController()->update();

	config["color"]["mode"] = m;
	saveConfig();

	responseMessage["code"] = 200;
	responseMessage["message"] = "success";
}

void HttpServer::setBrightnessAction(int b) {
	getLEDController()->setBrightness(b);
	getLEDController()->update();

	config["lights"]["brightness"] = b;
	saveConfig();

	responseMessage["code"] = 200;
	responseMessage["message"] = "success";
}

void HttpServer::setIsEnabledAction(bool e) {
	getLEDController()->setIsEnabled(e);
	getLEDController()->update();

	config["lights"]["is_enabled"] = e;
	saveConfig();

	responseMessage["code"] = 200;
	responseMessage["message"] = "success";
}

void HttpServer::loop() {
	server->handleClient();
}

HttpServer* getHttpServer() {
	if (httpServer == NULL) {
		httpServer = new HttpServer();
	}
	return httpServer;
}
