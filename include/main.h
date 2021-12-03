#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <StreamUtils.h>

#include "LittleFS.h"

extern FS LittleFS;
using littlefs_impl::LittleFSConfig;
extern FS* filesystem;
#define FileFS LittleFS
#define FS_NAME "LittleFS"

void setupESP();
File seekFile(const String& filename, size_t position = 0);
const String writeFile(const String& filename, const String& str);
const String readFile(const String& filename, size_t max_size);
const String filepath(const String& filename);
bool fileSystemInit();
String prettyBytes(size_t size);