
#include "config.h"
#include "util.h"

File logFile; 

void log(String l) {
	Serial.println(l);
	if(logFile) {
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
	if(LittleFS.begin()) {
		log("Filesystem mounted");
	}
	else {
		log("Filesystem mounting failed");
	}
	
}



void initLogs() {
	// truncate the log file
	logFile = LittleFS.open(LOG_FILE, "w+");
}

String getFileContents(String fileName) {
	String c;
	File f = LittleFS.open(fileName, "r");
	if(f) {
		c = f.readString();
		f.close();
	}
	else {
		log("Couldn't open file: " + fileName);
	}
	return c;
}

bool writeToFile(String fileName, String contents) {
	log("Writing to file " + fileName + ": " + contents);
	bool s;
	File f = LittleFS.open(fileName, "w");
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
