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
		String l = getFileContents("/" + server->arg("n"));
		l.replace("\n","<br>");
		sendHTMLResponse(l, 200);
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

		/*
			{
				"action": "set-single-color",
				"color": "0x00FF00"
			}
		*/
		else if(requestJSON["action"] == "set-single-color")  {
			if(requestJSON["color"] != "") {
				
				log(":::HTTP::: Setting single color to " + String(requestJSON["color"]));
				setSingleColorAction(requestJSON["color"].as<String>());
				responseMessage["code"] = 200;
				responseMessage["message"] = "success";
			}
			else {
				responseMessage["code"] = 500;
				responseMessage["message"] = "Color is required";
			}
		}

		/*
			{
				"action": "set-multi-static",
				"colors": [
					"0xFF0000",
					"0xFFFFFF",
					"0x0000FF"
				]
			}

		*/

		else if(requestJSON["action"] == "set-multi-static" && requestJSON["colors"]) {
			setMultiColorStaticAction(requestJSON["colors"].as<JsonArray>());
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}

		/*
			{
				"action": "set-multi-step",
				"colors": [
					"0xFF0000",
					"0xFFFFFF",
					"0x0000FF"
				]
			}

		*/

		else if(requestJSON["action"] == "set-multi-step" && requestJSON["colors"]) {
			setMultiColorStepAction(requestJSON["colors"].as<JsonArray>());
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}

		else if(requestJSON["action"] == "set-gradient-fade" && requestJSON["colors"]) {
			log("gradient request");
			setGradientFadeAction(requestJSON["colors"].as<JsonArray>());
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}

		else if(requestJSON["action"] == "set-data-pin") {
			config["data_pin"] = server->arg("p");
			saveConfig();
			responseMessage["code"] = 200;
			responseMessage["message"] = "success";
		}

		else if(requestJSON["action"] == "set-light-count") {
			log("setting light count to " + String(requestJSON["count"]));
			setLightCountAction(requestJSON["count"].as<int>());
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

void HttpServer::setSingleColorAction(String c) {
	log(":::HTTP::: Int color is " + String(c));
	getLEDController()->setMode(LIGHT_MODE_SINGLE_COLOR);
	getLEDController()->getColors()->clear();
	getLEDController()->getColors()->add(c);
	getLEDController()->update();
	config["color"]["mode"] = LIGHT_MODE_SINGLE_COLOR;
	config["color"]["colors"] = c;
	saveConfig();
};

void HttpServer::setMultiColorStaticAction(JsonArray c) {
	getLEDController()->setMode(LIGHT_MODE_MULTI_COLOR_STATIC);
	getLEDController()->setColors(colorJSONToList(c));
	getLEDController()->update();
	config["color"]["mode"] = LIGHT_MODE_MULTI_COLOR_STATIC;
	config["color"]["colors"] = c;
	saveConfig();
}

void HttpServer::setMultiColorStepAction(JsonArray c) {
	log("Multi step color begin");
	getLEDController()->setMode(LIGHT_MODE_MULTI_COLOR_STEP);
	getLEDController()->setColors(colorJSONToList(c));
	getLEDController()->update();
	config["color"]["mode"] = LIGHT_MODE_MULTI_COLOR_STEP;
	config["color"]["colors"] = c;
	saveConfig();
}

void HttpServer::setGradientFadeAction(JsonArray c) {
	log("Gradient begin");
	getLEDController()->setMode(LIGHT_MODE_GRADIENT_FADE);
	getLEDController()->setColors(colorJSONToList(c));
	getLEDController()->update();
	config["color"]["mode"] = LIGHT_MODE_GRADIENT_FADE;
	config["color"]["colors"] = c;
	saveConfig();
}

void HttpServer::setLightCountAction(int newLightCount) {
	getLEDController()->setCount(newLightCount);
	getLEDController()->update();
	saveConfig();
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