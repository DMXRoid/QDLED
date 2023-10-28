#include "FS.h"

#include "config.h"
#include "util.h"
File logFile; 

void log(String l) {
	Serial.println(l);
	if(true || logFile) {
		int b = logFile.println(l);
		if(!b) {
			Serial.println("Could not write to log file.	Log line: " + l);
		}
	}
	else {
		Serial.println("Could not open log file. Log line: " + l);
	}
}

void initFS() {
	if(SPIFFS.begin()) {
		log("Filesystem mounted");
	}
	else {
		log("Filesystem mounting failed");
	}
	
}



void initLogs() {
	// truncate the log file
	logFile = SPIFFS.open(LOG_FILE, "w+");
	if(!logFile) {
		Serial.println("Couldn't create log file");
	}
}

String getFileContents(String fileName) {
	String c;
	File f;
	if(fileName == LOG_FILE) {
		f = logFile;
		f.seek(0, fs::SeekSet);
	}
	else {
		f = SPIFFS.open(fileName, "r");
	}
	if(f) {
		c = f.readString();
		if (fileName != LOG_FILE) {
			f.close();
		}
	}
	else {
		log("Couldn't open file: " + fileName);
	}
	return c;
}

bool writeToFile(String fileName, String contents) {
	log("Writing to file " + fileName + ": " + contents);
	bool s;
	File f = SPIFFS.open(fileName, "a+");
	if(f) {
		int b = f.print(contents);
		if(!b) {
			log("Couldn't write to file: " + fileName);
		}
		else {
			s = true;
		}
		f.close();
	}
	else {
		log("Couldn't open file: " + fileName);
	}
	return s;
}

void mergeJSON(JsonVariant dst, JsonVariantConst src)
{
  if (src.is<JsonObjectConst>())
  {
    for (JsonPairConst kvp : src.as<JsonObjectConst>())
    {
      if (dst[kvp.key()]) 
        mergeJSON(dst[kvp.key()], kvp.value());
      else
        dst[kvp.key()] = kvp.value();
    }
  }
  else
  {
    dst.set(src);
  }
}
