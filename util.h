#include <WString.h>
#include <HardwareSerial.h>
#include <LittleFS.h>



String getFileContents(String);
bool writeToFile(String, String);
void log(String);
void initFS();
void initLogs();