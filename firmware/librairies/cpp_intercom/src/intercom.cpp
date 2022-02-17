#include "intercom.h"
#include "maths.h"
#include <ArduinoJson.h>

bool Intercom::_initialized = false;
String Intercom::_deviceId = "";
uint8_t Intercom::_lastCommand = 0;

StaticJsonDocument<1024> Intercom::_lastJsonDocument;
uint8_t Intercom::_messageCounter = 0;
ReceivedMessage Intercom::_receivedMessages[MAX_RECEIVED_MESSAGES];

uint8_t Intercom::_eventCounter = 0;
String Intercom::_receivedEvents[MAX_RECEIVED_EVENTS];

uint8_t Intercom::_servoCounter = 0;
RegisteredServo _servos[MAX_REGISTERED_SERVO];

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
        _receivedEvents[i] = "";
    }
}

void Intercom::sendDeviceId() {
    Serial.print(F("{\"c\":0,\"v\":\""));
    Serial.print(Intercom::_deviceId);
    Serial.println(F("\"}"));
}

bool Intercom::internalReceive(uint8_t max = MAX_RECEIVED_MESSAGES) {
    deserializeJson(_lastJsonDocument, Serial.readStringUntil('\n'));

    _lastCommand = _lastJsonDocument["c"];
    if(_lastCommand == 0) {
        sendDeviceId();
    } else if(_lastCommand == 2) {
        uint8_t type = _lastJsonDocument["t"].as<uint8_t>();
        uint8_t topic = _lastJsonDocument["s"].as<uint32_t>(); // s for subject

        int8_t length = -1;
        if(_lastJsonDocument.containsKey("l"))
            length = _lastJsonDocument["l"].as<int8_t>();

        uint8_t pos = _messageCounter;
        for(uint8_t i = 0; i < MAX_RECEIVED_MESSAGES; i++) {
            if(!_receivedMessages[i].occupied()) break;
            pos = (pos + 1) % MAX_RECEIVED_MESSAGES;
        }

        _messageCounter = pos;

        _receivedMessages[pos].topic = topic;
        _receivedMessages[pos].type = type;
        _receivedMessages[pos].length = length;

        bool isArray = length >= 0;

        if(_receivedMessages[pos].occupied())
            _receivedMessages[pos].release(); // IMPORTANT!

        if(type == 0) {
            // int
            if(isArray) {
                int* ptr = (int*)_receivedMessages[pos].alloc(sizeof(int) * length);
                for(int i = 0; i < length; i++) {
                    ptr[i] = _lastJsonDocument["v"][i].as<int>();
                }
            } else {
                int* ptr = (int*)_receivedMessages[pos].alloc(sizeof(int));
                *ptr = _lastJsonDocument["v"].as<int>();
            }
        } else if(type == 1) {
            // string
            if(isArray) {
                String* ptr = (String*)_receivedMessages[pos].alloc(sizeof(String) * length);
                for(int i = 0; i < length; i++) {
                    ptr[i] = _lastJsonDocument["v"][i].as<String>();
                }
            } else {
                String* ptr = (String*)_receivedMessages[pos].alloc(sizeof(String));
                *ptr = _lastJsonDocument["v"].as<String>();
            }
        } else if(type == 2) {
            // float
            if(isArray) {
                float* ptr = (float*)_receivedMessages[pos].alloc(sizeof(float) * length);
                for(int i = 0; i < length; i++) {
                    ptr[i] = _lastJsonDocument["v"][i].as<float>();
                }
            } else {
                float* ptr = (float*)_receivedMessages[pos].alloc(sizeof(float));
                *ptr = _lastJsonDocument["v"].as<float>();
            }
        }

        return true; // true if a real message was received
    } else if(_lastCommand == 3) {
        String eventName = _lastJsonDocument["e"].as<String>();
        _receivedEvents[_eventCounter] = eventName;
        _eventCounter = (_eventCounter + 1) % MAX_RECEIVED_EVENTS;
    }
    
    return false;
}

void Intercom::tick() {
    for(uint8_t i = 0; Serial.available() > 0 && i < MAX_RECEIVED_MESSAGES; i++) {
        internalReceive();
    }

    int angle = 0;
    for(uint8_t i = 0; i < _servoCounter; i++) {
        String servoId = _servos[i].servoId;
        if(instantReceiveInt("api_request_servo" + servoId, &angle)) {
            _servos[i].servo.write(angle);
        }
    }
}

void Intercom::waitForConnection() {
    while(true) {
        internalReceive(1);
        if(_lastCommand == 1 || _receivedMessages[_messageCounter].occupied())
            break;
    }
}

bool Intercom::waitForPing(unsigned int timeoutMs) {
    unsigned long start = millis();
    while(true) {
        internalReceive(1);
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

        if(waitForPing(250))
            break;
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

bool Intercom::instantReceiveInt(String topic, int* value) {
    uint32_t crc = crc24(topic);

    for(uint8_t i = 0; i < MAX_RECEIVED_MESSAGES; i++) {
        uint8_t pos = (_messageCounter + i) % MAX_RECEIVED_MESSAGES;
        ReceivedMessage message = _receivedMessages[pos];

        if(message.occupied() && message.topic == crc) {
            *value = *(int*)message.ptr;
            message.release();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveString(String topic, String* value) {
    uint32_t crc = crc24(topic);

    for(uint8_t i = 0; i < MAX_RECEIVED_MESSAGES; i++) {
        uint8_t pos = (_messageCounter + i) % MAX_RECEIVED_MESSAGES;
        ReceivedMessage message = _receivedMessages[pos];

        if(message.occupied() && message.topic == crc) {
            *value = *(String*)message.ptr;
            message.release();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveFloat(String topic, float* value) {
    uint32_t crc = crc24(topic);

    for(uint8_t i = 0; i < MAX_RECEIVED_MESSAGES; i++) {
        uint8_t pos = (_messageCounter + i) % MAX_RECEIVED_MESSAGES;
        ReceivedMessage message = _receivedMessages[pos];

        if(message.occupied() && message.topic == crc) {
            *value = *(float*)message.ptr;
            message.release();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveIntArray(String topic, int* ptr, int* length) {
    uint32_t crc = crc24(topic);

    for(uint8_t i = 0; i < MAX_RECEIVED_MESSAGES; i++) {
        uint8_t pos = (_messageCounter + i) % MAX_RECEIVED_MESSAGES;
        ReceivedMessage message = _receivedMessages[pos];

        if(message.occupied() && message.topic == crc) {
            *length = message.length;
            memcpy(ptr, message.ptr, message.length * sizeof(int));
            message.release();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveStringArray(String topic, String* ptr, int* length) {
    uint32_t crc = crc24(topic);

    for(uint8_t i = 0; i < MAX_RECEIVED_MESSAGES; i++) {
        uint8_t pos = (_messageCounter + i) % MAX_RECEIVED_MESSAGES;
        ReceivedMessage message = _receivedMessages[pos];

        if(message.occupied() && message.topic == crc) {
            *length = message.length;
            memcpy(ptr, message.ptr, message.length * sizeof(String));
            message.release();
            return true;
        }
    }

    return false;
}

bool Intercom::instantReceiveFloatArray(String topic, float* ptr, int* length) {
    uint32_t crc = crc24(topic);

    for(uint8_t i = 0; i < MAX_RECEIVED_MESSAGES; i++) {
        uint8_t pos = (_messageCounter + i) % MAX_RECEIVED_MESSAGES;
        ReceivedMessage message = _receivedMessages[pos];

        if(message.occupied() && message.topic == crc) {
            *length = message.length;
            memcpy(ptr, message.ptr, message.length * sizeof(float));
            message.release();
            return true;
        }
    }

    return false;
}

bool Intercom::instantHasReceivedEvent(String eventName) {
    if(eventName == "") return false;

    for(uint8_t i = _eventCounter; i < MAX_RECEIVED_EVENTS + _eventCounter; i++) {
        if(_receivedEvents[i % MAX_RECEIVED_EVENTS] == eventName) {
            _receivedEvents[i % MAX_RECEIVED_EVENTS] = "";
            return true;
        }
    }

    return false;
}

void Intercom::clearReceivedEvents() {
    for(uint8_t i = 0; i < MAX_RECEIVED_EVENTS; i++) {
        _receivedEvents[i] = "";
    }
}

void Intercom::publishEvent(String eventName) {
    Serial.print(F("{\"c\":3,\"e\":\""));
    Serial.print(eventName);
    Serial.println(F("\"}"));
}

void Intercom::registerServo(String servoId, int pin) {
    if(_servoCounter < MAX_REGISTERED_SERVO) {
        subscribe("api_request_servo" + servoId);

        _servos[_servoCounter].servoId = servoId;
        _servos[_servoCounter].servo.attach(pin);
        _servoCounter++;
    }
}

void Intercom::registerSensor(String sensorId) {
    subscribe("api_request_sensor" + sensorId);
}

int Intercom::isSensorRequested(String sensorId) {
    int readId = -1;
    instantReceiveInt("api_request_sensor" + sensorId, &readId);
    
    return readId;
}

void Intercom::sendSensorValue(String sensorId, int readId, int value) {
    int values[] = {readId, value};
    publish("api_response_sensor" + sensorId, values, 2);
}

void Intercom::sendSensorValue(String sensorId, int readId, float value) {
    float values[] = {(float)readId, value};
    publish("api_response_sensor" + sensorId, values, 2);
}
