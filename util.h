#include <WString.h>
#include <HardwareSerial.h>
#include <FS.h>



String getFileContents(String);
bool writeToFile(String, String);
void log(String);
void initFS();
void initLogs();