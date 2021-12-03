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
    File file1 = seekFile("/setup.json");           //читаем первый файл из памяти стримом
    File file2 = FileFS.open("/setup2.json", "w");  //создаем второй файл

    ReadBufferingStream bfile1{file1, 64};  //стримим первый файл

    WriteBufferingStream bfile2(file2, 64);  //записываем стрим

    DynamicJsonDocument doc(1024);

    Serial.println("during " + prettyBytes(ESP.getFreeHeap()));

    int i = 0;

    bfile1.find("[");
    do {
        i++;
        DeserializationError error = deserializeJson(doc, bfile1);

        doc["web"]["order"] = 10;

        serializeJson(doc, bfile2);

        if (error) {
            Serial.print("json error: ");
            Serial.println(error.f_str());
        }

        Serial.println(
            String(i) + ") " +
            doc["type"].as<String>() + " " +
            doc["set"]["gpio"].as<String>() + " " +
            doc["web"]["order"].as<String>());

    } while (bfile1.findUntil(",", "]"));

    Serial.println(readFile("/setup2.json", 20000));
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

const String writeFile(const String& filename, const String& str) {
    String path = filepath(filename);
    auto file = FileFS.open(path, "w");
    if (!file) {
        return "failed";
    }
    file.print(str);
    file.close();
    return "sucсess";
}

const String readFile(const String& filename, size_t max_size) {
    String path = filepath(filename);
    auto file = FileFS.open(path, "r");
    if (!file) {
        return "failed";
    }
    size_t size = file.size();
    if (size > max_size) {
        file.close();
        return "large";
    }
    String temp = file.readString();
    file.close();
    return temp;
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

// void readSDData() {
//     DynamicJsonDocument doc(1024);
//     myFileSDCart = SD.open(filename);
//
//     if (myFileSDCart) {
//         File filetmp = SD.open(filenametmp, FILE_WRITE);
//         WriteBufferingStream bufferedFileTmp(filetmp, 64);
//
//         ReadBufferingStream bufferedFile{myFileSDCart, 64};
//         bufferedFile.find("[");
//         do {
//             deserializeJson(doc, bufferedFile);
//             // Тут правим если надо
//
//             serializeJson(doc, bufferedFileTmp);
//
//             serializeJson(doc, Serial);
//             Serial.println();
//             Serial.println(doc["type"].as<String>());
//         } while (bufferedFile.findUntil(",", "]"));

//         // close the file:
//         myFileSDCart.close();
//
//         bufferedFileTmp.flush();
//         filetmp.close();

//         // FS.rename(filenametmp, filename);
//         /*
//         int rc = rename(filenametmp, filename);
//         if (rc != 0) {
//             ESP_LOGE(TAG, "Rename failed, errno=%d (%s)", errno, strerror(errno));
//         }
//         */
//
//     } else {
//         // if the file didn't open, print an error:
//         Serial.print(F("Error opening (or file not exists) "));
//         Serial.println(filename);
//     }
//     lastSDRead = millis();
// }