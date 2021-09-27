#include "main.h"

void setup() {
    Serial.begin(115200);
    Serial.flush();
    Serial.println();
    Serial.println(F("--------------started----------------"));
    fileSystemInit();

    Serial.println("before " + prettyBytes(ESP.getFreeHeap()));
    setupESP();
    Serial.println("after " + prettyBytes(ESP.getFreeHeap()));
}

void loop() {
}

void setupESP() {
    File file = seekFile("/setup.json");
    ReadBufferingStream bufferedFile{file, 64};
    DynamicJsonDocument doc(1024);
    Serial.println("during " + prettyBytes(ESP.getFreeHeap()));
    int i = 0;
    bufferedFile.find("[");
    do {
        i++;
        DeserializationError error = deserializeJson(doc, bufferedFile);
        //serializeJson(doc, Serial);
        if (error) {
            Serial.print("json error: ");
            Serial.println(error.f_str());
        }
        Serial.println(String(i) + ") " + doc["type"].as<String>() + " " + doc["set"]["gpio"].as<String>());
    } while (bufferedFile.findUntil(",", "]"));
}

File seekFile(const String& filename, size_t position) {
    String path = filepath(filename);
    auto file = FileFS.open(path, "r");
    if (!file) {
        Serial.println("[E] file error");
    }
    file.seek(position, SeekSet);
    return file;
}

const String filepath(const String& filename) {
    return filename.startsWith("/") ? filename : "/" + filename;
}

bool fileSystemInit() {
    if (!FileFS.begin()) {
        Serial.println("FS Init ERROR, may be FS was not flashed");
        return false;
    }
    Serial.println("FS Init completed");
    return true;
}

String prettyBytes(size_t size) {
    if (size < 1024)
        return String(size) + "b";
    else if (size < (1024 * 1024))
        return String(size / 1024.0) + "kB";
    else if (size < (1024 * 1024 * 1024))
        return String(size / 1024.0 / 1024.0) + "MB";
    else
        return String(size / 1024.0 / 1024.0 / 1024.0) + "GB";
}