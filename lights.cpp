#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wregister"


#include "config.h"
#include "util.h"
#include "lights.h"

LEDs* ledController;

LEDs::LEDs(int newLEDCount, int newMode, LinkedList<String>* newColors) {
	isGradient = false;
	colors = new LinkedList<CRGB>();
	preprocessedColors = new LinkedList<CRGB>();
	isGradient = (config["color"]["is_gradient"]) ? true : false;
	isEnabled = (config["lights"]["is_enabled"]) ? true : false;
	log("setting led count");
	setCount(newLEDCount);
	log("setting led colors");
	setColors(newColors);
	setBrightness(config["lights"]["brightness"]);


	log("setting led mode");
	setMode(newMode);
	log("setting led delays");
	stepDelay = config["color"]["step_delay"];
	fadeDelay = config["color"]["fade_delay"];

	log("allocating LEDs");
	log("pin setup: pin #: " + config["data_pin"].as<String>());
	switch (config["data_pin"].as<int>()) {
		case 1:
		FastLED.addLeds<WS2812B, 1, GRB>(leds, 200);
		break;
		case 2:
		FastLED.addLeds<WS2812B, 2, GRB>(leds, 100);
		break;
		case 3:
		FastLED.addLeds<WS2812B, 3, GRB>(leds, 100);
		break;
		case 4:
		FastLED.addLeds<WS2812B, 4, GRB>(leds, 100);
		break;
		case 5:
		FastLED.addLeds<WS2812B, 5, GRB>(leds, 100);
		break;
	
		case 12:
		FastLED.addLeds<WS2812B, 12, GRB>(leds, 100);
		break;
		case 13:
		FastLED.addLeds<WS2812B, 13, GRB>(leds, 100);
		break;
		case 14:
		FastLED.addLeds<WS2812B, 14, GRB>(leds, 100);
		break;
		case 15:
		FastLED.addLeds<WS2812B, 15, GRB>(leds, 100);
		break;
		case 16:
		FastLED.addLeds<WS2812B, 16, GRB>(leds, 100);
		break;
	}
	update();
}

void LEDs::updateFromConfig() {
	setColors(colorJSONToList(config["color"]["colors"].as<JsonArray>()));
	setCount(config["lights"]["count"]);
	setMode(config["color"]["mode"]);
	setFadeDelay(config["color"]["fade_delay"]);
	setStepDelay(config["color"]["step_delay"]);
	isGradient = (config["color"]["is_gradient"]) ? true : false;
	isEnabled = (config["lights"]["is_enabled"]) ? true : false;
	update();
}

void LEDs::setCount(int newLEDCount) {
	ledCount = newLEDCount;
}

void LEDs::setMode(int newMode) {
	mode = newMode;
}

void LEDs::setStepDelay(int sd) {
	stepDelay = sd;
}

void LEDs::setFadeDelay(int fd) {
	fadeDelay = fd;
}

void LEDs::setBrightness(int b) {
	brightness = b;
	FastLED.setBrightness(brightness);
}

void LEDs::setIsEnabled(bool e) {
	isEnabled = e;
}

void LEDs::setColors(LinkedList<String>* newColors) {
	log("adding colors");
	if(colors->size() > 0) {
		log("clearing current colors");
		colors->clear();
	}
	
	for(int x = 0; x < newColors->size(); x++) {
		log("adding color:::" + newColors->get(x));
		colors->add(stringToCRGB(newColors->get(x)));
	}
	
	preprocessedColors = colors;
	log("finished adding colors");
}

LinkedList<CRGB>* LEDs::getColors() {
	return colors;
}

void LEDs::setIsGradient(bool ig) {
	isGradient = ig;
}

void LEDs::update() {
	log("Starting update...");
	if(isEnabled) {
		if(isGradient && colors->size() > 1) {
			if(preprocessedColors->size() > 0) {
				colors = preprocessedColors;
			}
			setupGradient();
			int stepSize = 1;
			if(mode != LIGHT_MODE_FADE) {
				stepSize = floor(255 / ledCount);
			}
			log("step size is ::: " + String(stepSize));
			processGradient(stepSize);
		}
		switch (mode) {
			case LIGHT_MODE_STATIC:
			staticLights();
			break;

			case LIGHT_MODE_STEP:
			stepLights();
			break;

			case LIGHT_MODE_FADE:
			fadeLights();
			break;

			case LIGHT_MODE_WHOLE_STEP:
			wholeStepLights();
			break;
		}
	}
	else {
		FastLED.clear(true);
		FastLED.show();
	}
	
}

void LEDs::staticLights() {
	FastLED.setBrightness(config["lights"]["brightness"]);
	int y;
	int cs = colors->size();
	for (int x = 0; x < ledCount; x++) {
		y = (x % ledCount) % cs;
		leds[x] = colors->get(y);
	}
	FastLED.show();
	FastLED.show();
}

void LEDs::stepLights() {
	int y;
	int cs = colors->size();
	if (cs < 2) {
		staticLights();
	}
	for (int x = 0; x < ledCount; x++) {
		y = (((x % ledCount) % cs) + step) % cs;
		leds[x] = colors->get(y);
	}
	FastLED.show();
	FastLED.show();
	step++;
}

void LEDs::wholeStepLights() {
	int cs = colors->size();
	int y;
	y = step % cs;
	singleColor(colors->get(y));
	step++;
}


void LEDs::loop() {
	if(shouldRun()) {
		switch(mode) {
			case LIGHT_MODE_STATIC:
			break;

			
			case LIGHT_MODE_STEP:
				stepLights();
			break;

			case LIGHT_MODE_FADE:
				fadeLights();
			break;

			case LIGHT_MODE_WHOLE_STEP:
				wholeStepLights();
			break;

		}
		lastRunTime = millis();
	}
}

void LEDs::emptyLoop() {}

void LEDs::fadeLights() {
	// progress "forward"
	if ( fadeDirection == 0) {
		//log("step " + String(step) + " ::: Size: " + String(gradientStepSize));
		if (step >= colors->size()) {
	//		log("Turning over");
			fadeDirection = 1;
			step = colors->size() - 1;
		}
		else {
			step++;
		}
	}
	else {
		if (step == 0) {
			fadeDirection = 0;
		}
		else {
			step--;
		}
	}

	singleColor(colors->get(step));
}



void LEDs::processGradient(int stepSize = 1) {
	preprocessedColors = colors;
	colors->clear();
	for (int x = 0; x <= 255; x++) {
		if (x % stepSize == 0) {
			colors->add(ColorFromPalette(gradientPalette, (( x <= 248 ) ? x : 248 ), 255));
		}
	}
}

void LEDs::setupGradient() {
	log("Setting up gradient");
	int gss = ceil(256 / (colors->size() - 1));

	log("Gradient step size:::" + String(gss));
	

	unsigned char gradientColors[4 * colors->size()];
	for ( int x = 0; x < colors->size(); x++) {
		int y = x * 4;
		gradientColors[y + 0] = (x == colors->size() - 1) ? 255 : (x * gss);
		gradientColors[y + 1] = colors->get(x).red;
		gradientColors[y + 2] = colors->get(x).green;
		gradientColors[y + 3] = colors->get(x).blue;

		//log("gradient array ::: " + String(gradientColors[y+0]) + ", " + String(gradientColors[y+1]) + ", " + String(gradientColors[y+2]) + ", " + String(gradientColors[y+3]) + ", ");
	}

	log("Loading gradient palette");
	gradientPalette.loadDynamicGradientPalette(gradientColors);
	
	log("gradient palette loaded");
	step = 0;
	fadeDirection = 0;
}

bool LEDs::shouldRun() {
	return isEnabled && (mode != LIGHT_MODE_STATIC) && (millis() - lastRunTime) > ((mode == LIGHT_MODE_FADE) ? fadeDelay : stepDelay);
}

long int stringToColor(String color) {
	return strtol(color.c_str(), NULL, 16);
}

CRGB stringToCRGB(String c) {
	CRGB rgb(strtol(c.substring(2,8).c_str(), NULL, 16));
	//log("Color " + c + " ::: r: " + rgb.red + "::: g: " + rgb.green + " ::: b: " + rgb.blue);
	return rgb;
}

LinkedList<String>* colorJSONToList(JsonArray cArray) {
	LinkedList<String> *c = new LinkedList<String>();
	for (JsonVariant v : cArray) {
	//	log("Adding color " + v.as<String>());
		c->add(v.as<String>());
	}
	
	return c;
}



LEDs* getLEDController() {
	return ledController;
}
void initLEDs() {
	ledController = new LEDs(config["lights"]["count"].as<int>(), config["color"]["mode"].as<int>(), colorJSONToList(config["color"]["colors"].as<JsonArray>()));
}



// mostly deprecated 

void LEDs::singleColor() {
	//log("Setting single color...");

	for (int x = 0; x < ledCount; x++) {
		leds[x] = colors->get(0);
	}
	FastLED.show();
	FastLED.show();
}

void LEDs::singleColor(CRGB c) {
	//log("Specified single color::: R: " + String(c.red)  + ", G: " + String(c.green) + ", B: " + String(c.blue));
	for (int x = 0; x < ledCount; x++) {
		leds[x] = c;
		
	}
	FastLED.show();
	FastLED.show();
}

void LEDs::multiColorStatic() {
	int y;
	int cs = colors->size();
	for (int x = 0; x < ledCount; x++) {
		y = (x % ledCount) % cs;
		leds[x] = colors->get(y);
	}
	FastLED.show();
	FastLED.show();
}

void LEDs::multiColorStep() {
	int y;
	int cs = colors->size();
	for (int x = 0; x < ledCount; x++) {
		y = (((x % ledCount) % cs) + step) % cs;

		leds[x] = colors->get(y);
	}
	FastLED.show();
	FastLED.show();
	delay(stepDelay);
	step++;
}

void LEDs::multiColorStepLoop() {
	multiColorStep();
}

/*

void LEDs::gradientFadeLoop() {
	int i;
	

	// progress "forward"
	if ( gradientDirection == 0) {
		i = gradientStepSize * step;
		//log("step " + String(step) + " ::: Size: " + String(gradientStepSize));
		if (i > 255) {
	//		log("Turning over");
			i = 255; 
			gradientDirection = 1;
			step = 0;
		}
		else {
			step++;
		}
	}
	else {
		i = 255 - (gradientStepSize * step);
		if (i < 0) {
			i = 0;
			gradientDirection = 0;
			step = 0;
		}
		else {
			step++;
		}
	}

	// absolute bullshit hack b/c for some reason the last 5 frames or whatever
	// of the generated palette transition back to the first color 

	i = (i > 248) ? 248 : i;
	//log("gradient palette index: " + String(i));
	singleColor(ColorFromPalette(gradientFadePalette, i, 255));
	delay(fadeDelay);
}*/

CHSV RGBtoHSV (CRGB rgb) {
	double hsv[3];

	byte xMin = rgb[0];
	if (rgb[1]<xMin) {
		xMin = rgb[1];
	}
	if (rgb[2]<xMin) {		 xMin = rgb[2];	 }	 byte xMax = rgb[0];	 if (rgb[1]>xMax) {
		xMax = rgb[1];
	}
	if (rgb[2]>xMax) {
		xMax = rgb[2];
	}
	hsv[2] = xMax;
 
	byte delta = xMax-xMin;
 
	if (xMax!=0) {
		hsv[1] = (int)(delta)*255/xMax;
	} else {
		hsv[0] = 0;
		hsv[1] = 0;
		return CHSV(0,0,0);
	}
 
	if (rgb[0]==xMax) {
		hsv[0] = (rgb[1]-rgb[2])*60/delta;
	} else if (rgb[1]==xMax) {
		hsv[0] = 120+(rgb[2]-rgb[0])*60/delta;
	} else {
		hsv[0] = 240+(rgb[0]-rgb[1])*60/delta;
	}
	if (hsv[0]<0) {
		hsv[0] += 360;
	}
	return CHSV(hsv[0], hsv[1], hsv[2]);
}

void HSVtoRGB (double hsv[], byte rgb[]) {
	double h = hsv[0];
	double s = hsv[1]/255.0;
	double v = hsv[2]/255.0;
	double c = v*s;
	double tmp = h/60.0;
	double tmp2 = tmp-2*floor(tmp/2);
	double x = c*(1-abs(tmp2-1));
	double m = v-c;
	double r,g,b;
	int i = floor(tmp);

	switch (i) {
		case 0:
			r = c;
			g = x;
			b = 0;
			break;
		case 1:
			r = x;
			g = c;
			b = 0;
			break;
		case 2: 
			r = 0;
			g = c;
			b = x;
			break;
		case 3:
			r = 0;
			g = x;
			b = c;
			break;
		case 4:
			r = x;
			g = 0;
			b = c;
			break;
		case 5:
			r = c;
			g = 0;
			b = x;
			break;
	}
	rgb[0] = constrain((int)255*(r+m),0,255);
	rgb[1] = constrain((int)255*(g+m),0,255);
	rgb[2] = constrain((int)255*(b+m),0,255);
}

