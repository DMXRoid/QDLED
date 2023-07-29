#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wregister"

#include <FastLED.h>
#include <pixeltypes.h>
#include <LinkedList.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif


class LEDs;
extern struct CRGB leds[200];

class LEDs {
	public: 
		LEDs(int ledCount, int mode, LinkedList<String>* colors );
		void setMode(int mode);
		void setColors(LinkedList<String>*);
		void setCount(int);
		void setStepDelay(int);
		void setFadeDelay(int);
		void setBrightness(int);
		void setIsEnabled(bool);
		LinkedList<CRGB>* getColors();
		void update();
		void loop();
		void setIsGradient(bool);

		bool isGradient;
		

	private:
		int mode;
		int ledCount;
		int step; 
		int stepDelay;
		int fadeDelay;
		int brightness;
		bool isEnabled;

		unsigned long lastRunTime;

		LinkedList<CRGB> *colors;
		LinkedList<CRGB> *preprocessedColors;

		CRGBPalette32 gradientPalette;
		int fadeDirection;
		
		struct CRGB leds[200];

		void setupGradient();
		void processGradient(int);
		void staticLights();
		void stepLights();
		void fadeLights();

		
		void singleColor();
		void singleColor(CRGB);
		void multiColorStatic();
		void multiColorStep();
		void gradientFade();
		void gradientFadeLoop();
		void multiColorStepLoop();
		void emptyLoop();
		bool shouldRun();
	
		

		
};
LEDs* getLEDController();

void initLEDs();
void lightLoop();
long int stringToColor(String);
LinkedList<String>* colorJSONToList(JsonArray);
CRGB stringToCRGB(String);
CHSV RGBtoHSV(CRGB);
