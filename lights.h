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
extern struct CRGB leds[];

class LEDs {
	public: 
		LEDs(int ledCount, int mode, LinkedList<String>* colors );
		void setMode(int mode);
		void setColors(LinkedList<String>*);
		void setCount(int);
		LinkedList<String>* getColors();
		void update();
		void loop();

	private:
		int mode;
		int ledCount;
		int step; 
		LinkedList<String> *colors;

		CRGBPalette32 gradientFadePalette;
		int gradientDirection;
		int gradientStepSize = 2;

		struct CRGB *leds = NULL;

		
		void singleColor();
		void singleColor(CRGB);
		void multiColorStatic();
		void multiColorStep();
		void gradientFade();
		void gradientFadeLoop();
		void multiColorStepLoop();
		void emptyLoop();
		

		
};
LEDs* getLEDController();

void initLEDs();
void lightLoop();
long int stringToColor(String);
LinkedList<String>* colorJSONToList(JsonArray);
CRGB stringToCRGB(String);
CHSV RGBtoHSV(CRGB);