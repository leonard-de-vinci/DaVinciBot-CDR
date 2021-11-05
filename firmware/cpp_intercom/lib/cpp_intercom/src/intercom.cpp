#include "intercom.h"
#include "utils/maths.h"
#include <ArduinoJson.h>

bool Intercom::_initialized = false;
void Intercom::init(String deviceId, int speed = 115200) {
    if(_initialized)
        return;

    _initialized = true;
    _deviceId = deviceId;
    Serial.begin(speed);

    sendDeviceId();
    waitForConnection();
}

void Intercom::sendDeviceId() {
    Serial.print(F("{\"c\":0,\"v\":\""));
    Serial.print(_deviceId);
    Serial.println(F("\"}"));
}

void Intercom::internalReceive() {
    deserializeJson(_jsonDocument, Serial);

    _lastCommand = _jsonDocument["c"];
    if(_lastCommand == 0)
        sendDeviceId();
    
    if(_lastCommand == 2) {
        _lastType = _jsonDocument["t"];
        _lastTopic = _jsonDocument["s"]; // s for subject, t is already used by type
    } else {
        _lastType = 255;
        _lastTopic = 0;
    }
}

void Intercom::waitForConnection() {
    while(true) {
        internalReceive();
        if(_lastCommand == 1)
            break;
    }
}

bool Intercom::waitForPing(int timeoutMs) {
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
        if(_lastCommand == 2 && _lastType == 0)
            return _jsonDocument["v"];
    }
}

String Intercom::receiveString() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 1)
            return _jsonDocument["v"];
    }
}

float Intercom::receiveFloat() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2)
            return _jsonDocument["v"];
    }
}

int Intercom::receiveInt(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 0 && _lastTopic == crcTopic)
            return _jsonDocument["v"];
    }
}

String Intercom::receiveString(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 1 && _lastTopic == crcTopic)
            return _jsonDocument["v"];
    }
}

float Intercom::receiveFloat(String topic) {
    uint32_t crcTopic = crc24(topic);
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastTopic == crcTopic)
            return _jsonDocument["v"];
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
