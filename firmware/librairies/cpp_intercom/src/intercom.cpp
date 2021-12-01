#include "intercom.h"
#include "maths.h"
#include <ArduinoJson.h>

bool Intercom::_initialized = false;
String Intercom::_deviceId = "";

StaticJsonDocument<256> Intercom::_jsonDocument;
uint8_t Intercom::_lastCommand;
uint8_t Intercom::_lastType;
uint32_t Intercom::_lastTopic;
int8_t Intercom::_lastLength;

void Intercom::init(String deviceId, unsigned long speed) {
    if(_initialized)
        return;

    _initialized = true;
    Intercom::_deviceId = deviceId;
    Serial.begin(speed);

    sendDeviceId();
    waitForConnection();
}

void Intercom::sendDeviceId() {
    Serial.print(F("{\"c\":0,\"v\":\""));
    Serial.print(Intercom::_deviceId);
    Serial.println(F("\"}"));
}

void Intercom::internalReceive() {
    while(Serial.available() == 0);

    deserializeJson(_jsonDocument, Serial.readStringUntil('\n'));

    _lastCommand = _jsonDocument["c"];
    if(_lastCommand == 0)
        sendDeviceId();
    
    if(_lastCommand == 2) {
        _lastType = _jsonDocument["t"];
        _lastTopic = _jsonDocument["s"]; // s for subject, t is already used by type

        if(_jsonDocument.containsKey("l"))
            _lastLength = _jsonDocument["l"];
        else
            _lastLength = -1;
    } else {
        _lastType = 255;
        _lastTopic = 0;
        _lastLength = -1;
    }
}

void Intercom::waitForConnection() {
    while(true) {
        internalReceive();
        if(_lastCommand == 1)
            break;
    }
}

bool Intercom::waitForPing(unsigned int timeoutMs) {
    unsigned long start = millis();
    while(true) {
        internalReceive();
        if(_lastCommand == 1)
            return true;
        if(millis() - start > timeoutMs)
            return false;
    }
}

void Intercom::subscribe(String topic) {
    while(true) {
        Serial.print(F("{\"c\":2,\"t\":255,\"s\":"));
        Serial.print(crc24(topic));
        Serial.println(F("}"));

        if(waitForPing(500))
            break;
    }
}

int Intercom::receiveInt() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 0 && _lastLength == -1)
            return _jsonDocument["v"];
    }
}

String Intercom::receiveString() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 1 && _lastLength == -1)
            return _jsonDocument["v"];
    }
}

float Intercom::receiveFloat() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength == -1)
            return _jsonDocument["v"];
    }
}

double Intercom::receiveDouble() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength == -1)
            return _jsonDocument["v"];
    }
}

int Intercom::receiveInt(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        int val = receiveInt();
        if(_lastTopic == crcTopic)
            return val;
    }
}

String Intercom::receiveString(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        String val = receiveString();
        if(_lastTopic == crcTopic)
            return val;
    }
}

float Intercom::receiveFloat(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        float val = receiveFloat();
        if(_lastTopic == crcTopic)
            return val;
    }
}

double Intercom::receiveDouble(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        double val = receiveDouble();
        if(_lastTopic == crcTopic)
            return val;
    }
}

int* Intercom::receiveIntArray() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 0 && _lastLength >= 0) {
            int* arr = new int[_lastLength];
            for(int i = 0; i < _lastLength; i++)
                arr[i] = _jsonDocument["v"][i];
            return arr;
        }
    }
}

String* Intercom::receiveStringArray() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 1 && _lastLength >= 0) {
            String* arr = new String[_lastLength];
            for(int i = 0; i < _lastLength; i++)
                arr[i] = _jsonDocument["v"][i].as<String>();
            return arr;
        }
    }
}

float* Intercom::receiveFloatArray() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength >= 0) {
            float* arr = new float[_lastLength];
            for(int i = 0; i < _lastLength; i++)
                arr[i] = _jsonDocument["v"][i];
            return arr;
        }
    }
}

double* Intercom::receiveDoubleArray() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength >= 0) {
            double* arr = new double[_lastLength];
            for(int i = 0; i < _lastLength; i++)
                arr[i] = _jsonDocument["v"][i];
            return arr;
        }
    }
}

int* Intercom::receiveIntArray(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        int* arr = receiveIntArray();
        if(_lastTopic == crcTopic)
            return arr;
    }
}

String* Intercom::receiveStringArray(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        String* arr = receiveStringArray();
        if(_lastTopic == crcTopic)
            return arr;
    }
}

float* Intercom::receiveFloatArray(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        float* arr = receiveFloatArray();
        if(_lastTopic == crcTopic)
            return arr;
    }
}

double* Intercom::receiveDoubleArray(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        double* arr = receiveDoubleArray();
        if(_lastTopic == crcTopic)
            return arr;
    }
}

void Intercom::publish(String topic, int value) {
    Serial.print(F("{\"c\":2,\"t\":0,\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":"));
    Serial.print(value);
    Serial.println(F("}"));
}

void Intercom::publish(String topic, String value) {
    Serial.print(F("{\"c\":2,\"t\":1,\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":\""));
    Serial.print(value);
    Serial.println(F("\"}"));
}

void Intercom::publish(String topic, float value) {
    Serial.print(F("{\"c\":2,\"t\":2,\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":"));
    Serial.print(value);
    Serial.println(F("}"));
}

void Intercom::publish(String topic, double value) {
    // double has the same datatype as float
    Serial.print(F("{\"c\":2,\"t\":2,\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":"));
    Serial.print(value);
    Serial.println(F("}"));
}

void Intercom::publish(String topic, int* ptr, int length) {
    Serial.print(F("{\"c\":2,\"t\":0,\"l\":"));
    Serial.print(length);
    Serial.print(F(",\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":"));
    Serial.print(F("["));
    for(int i = 0; i < length; i++) {
        if(i > 0)
            Serial.print(F(","));
        Serial.print(ptr[i]);
    }
    Serial.println(F("]"));
}

void Intercom::publish(String topic, String* ptr, int length) {
    Serial.print(F("{\"c\":2,\"t\":1,\"l\":"));
    Serial.print(length);
    Serial.print(F(",\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":"));
    Serial.print(F("["));
    for(int i = 0; i < length; i++) {
        if(i > 0)
            Serial.print(F(","));
        Serial.print(F("\""));
        Serial.print(ptr[i]);
        Serial.print(F("\""));
    }
    Serial.println(F("]"));
}

void Intercom::publish(String topic, float* ptr, int length) {
    Serial.print(F("{\"c\":2,\"t\":2,\"l\":"));
    Serial.print(length);
    Serial.print(F(",\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":"));
    Serial.print(F("["));
    for(int i = 0; i < length; i++) {
        if(i > 0)
            Serial.print(F(","));
        Serial.print(ptr[i]);
    }
    Serial.println(F("]"));
}

void Intercom::publish(String topic, double* ptr, int length) {
    // double has the same datatype as float
    Serial.print(F("{\"c\":2,\"t\":2,\"l\":"));
    Serial.print(length);
    Serial.print(F(",\"s\":"));
    Serial.print(crc24(topic));
    Serial.print(F(",\"v\":"));
    Serial.print(F("["));
    for(int i = 0; i < length; i++) {
        if(i > 0)
            Serial.print(F(","));
        Serial.print(ptr[i]);
    }
    Serial.println(F("]"));
}
