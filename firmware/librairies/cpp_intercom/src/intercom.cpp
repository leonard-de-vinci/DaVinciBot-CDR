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
uint8_t Intercom::_eventCounter = 0;

void Intercom::init(String deviceId, unsigned long speed) {
    if(_initialized)
        return;

    _initialized = true;
    Intercom::_deviceId = deviceId;
    Serial.begin(speed);

    initEvents();
    sendDeviceId();
    waitForConnection();
}

void Intercom::initEvents() {
    for(uint8_t i = 0; i < MAX_RECEIVED_EVENTS; i++) {
        _lastReceivedEvents[i] = "";
    }
}

void Intercom::sendDeviceId() {
    Serial.print(F("{\"c\":0,\"v\":\""));
    Serial.print(Intercom::_deviceId);
    Serial.println(F("\"}"));
}

void Intercom::internalReceive() {
    while(Serial.available() == 0);

    deserializeJson(_jsonDocument, Serial.readStringUntil('\n'));

    _lastCommand = _jsonDocument["c"].as<uint8_t>();
    if(_lastCommand == 0)
        sendDeviceId();
    
    if(_lastCommand == 2) {
        _lastType = _jsonDocument["t"].as<uint8_t>();
        _lastTopic = _jsonDocument["s"].as<uint32_t>(); // s for subject, t is already used by type

        if(_jsonDocument.containsKey("l"))
            _lastLength = _jsonDocument["l"].as<int8_t>();
        else
            _lastLength = -1;
    } else {
        if(_lastCommand == 3)
            internalReceiveEvent();

        _lastType = 255;
        _lastTopic = 0;
        _lastLength = -1;
    }
}

void Intercom::internalReceiveEvent() {
    String eventName = _jsonDocument["e"].as<String>();
    _lastReceivedEvents[_eventCounter] = eventName;
    _eventCounter = (_eventCounter + 1) % MAX_RECEIVED_EVENTS;
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
            return _jsonDocument["v"].as<int>();
    }
}

String Intercom::receiveString() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 1 && _lastLength == -1)
            return _jsonDocument["v"].as<String>();
    }
}

float Intercom::receiveFloat() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength == -1)
            return _jsonDocument["v"].as<float>();
    }
}

double Intercom::receiveDouble() {
    while(true) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength == -1)
            return _jsonDocument["v"].as<double>();
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
                arr[i] = _jsonDocument["v"][i].as<int>();
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
                arr[i] = _jsonDocument["v"][i].as<float>();
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
                arr[i] = _jsonDocument["v"][i].as<double>();
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
    Serial.println(F("]}"));
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
    Serial.println(F("]}"));
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
    Serial.println(F("]}"));
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
    Serial.println(F("]}"));
}

bool Intercom::instantReceiveInt(String* topic, int* value) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 0 && _lastLength < 0) {
            *topic = _lastTopic;
            *value = _jsonDocument["v"].as<int>();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveString(String* topic, String* value) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 1 && _lastLength < 0) {
            *topic = _lastTopic;
            *value = _jsonDocument["v"].as<String>();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveFloat(String* topic, float* value) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength < 0) {
            *topic = _lastTopic;
            *value = _jsonDocument["v"].as<float>();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveDouble(String* topic, double* value) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength < 0) {
            *topic = _lastTopic;
            *value = _jsonDocument["v"].as<double>();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveIntArray(String* topic, int* ptr, int* length) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 0 && _lastLength > 0) {
            *topic = _lastTopic;
            *length = _lastLength;
            for(int i = 0; i < _lastLength; i++) {
                ptr[i] = _jsonDocument["v"][i].as<int>();
            }
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveStringArray(String* topic, String* ptr, int* length) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 1 && _lastLength > 0) {
            *topic = _lastTopic;
            *length = _lastLength;
            for(int i = 0; i < _lastLength; i++) {
                ptr[i] = _jsonDocument["v"][i].as<String>();
            }
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveFloatArray(String* topic, float* ptr, int* length) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength > 0) {
            *topic = _lastTopic;
            *length = _lastLength;
            for(int i = 0; i < _lastLength; i++) {
                ptr[i] = _jsonDocument["v"][i].as<float>();
            }
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveDoubleArray(String* topic, double* ptr, int* length) {
    if(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 2 && _lastType == 2 && _lastLength > 0) {
            *topic = _lastTopic;
            *length = _lastLength;
            for(int i = 0; i < _lastLength; i++) {
                ptr[i] = _jsonDocument["v"][i].as<double>();
            }
            return true;
        }
    }

    return false;
}

bool Intercom::instantHasReceivedEvent(String eventName) {
    if(eventName == "") return false;

    for(uint8_t i = _eventCounter; i < MAX_RECEIVED_EVENTS + _eventCounter; i++) {
        if(_lastReceivedEvents[i % MAX_RECEIVED_EVENTS] == eventName) {
            _lastReceivedEvents[i % MAX_RECEIVED_EVENTS] = "";
            return true;
        }
    }

    return false;
}

void Intercom::clearReceivedEvents() {
    for(uint8_t i = 0; i < MAX_RECEIVED_EVENTS; i++) {
        _lastReceivedEvents[i] = "";
    }
}

void Intercom::publishEvent(String eventName) {
    Serial.print(F("{\"c\":3,\"e\":\""));
    Serial.print(eventName);
    Serial.println(F("\"}"));
}

bool Intercom::hasReceivedEvent(String eventName) {
    if(instantHasReceivedEvent(eventName))
        return true;

    while(Serial.available() > 0) {
        internalReceive();
        if(_lastCommand == 3) {
            if(_lastReceivedEvents[_eventCounter] == eventName) {
                return true;
            }
        }
    }

    return false;
}
