#include <Arduino.h>
#include <ArduinoJson.h>
#include <PWMServo.h>

#define DEFAULT_INTERCOM_SPEED 115200
#define MAX_RECEIVED_MESSAGES 64
#define MAX_RECEIVED_EVENTS 16
#define MAX_REGISTERED_SERVO 16

struct ReceivedMessage {
    uint32_t topic;
    uint8_t type;
    int8_t length;
    void* ptr;

    bool occupied() {
        return ptr != NULL;
    }

    void* alloc(int length) {
        ptr = malloc(length);
        return ptr;
    }

    void release() {
        free(ptr);
        ptr = NULL;
    }
};

struct RegisteredServo {
    String servoId = "";
    PWMServo servo;
};

class Intercom {
    public: 
        static void init(String deviceId, unsigned long speed);
        static void tick();

        static void publish(String topic, int value);
        static void publish(String topic, String value);
        static void publish(String topic, float value);

        static void publish(String topic, int* ptr, int length);
        static void publish(String topic, String* ptr, int length);
        static void publish(String topic, float* ptr, int length);
 
        static void subscribe(String topic);

        // only works for subscribed topics
        static bool instantReceiveInt(String topic, int* value);
        static bool instantReceiveString(String topic, String* value);
        static bool instantReceiveFloat(String topic, float* value);
        static bool instantReceiveIntArray(String topic, int* ptr, int* length);
        static bool instantReceiveStringArray(String topic, String* ptr, int* length);
        static bool instantReceiveFloatArray(String topic, float* ptr, int* length);

        static bool instantHasReceivedEvent(String eventName);
        static void clearReceivedEvents();
        static void publishEvent(String eventName);

        static void registerServo(String servoId, int pin);
        static void registerSensor(String sensorId);

        static int isSensorRequested(String sensorId);
        static bool isSensorRequested(String sensorId, int* readId);
        static void sendSensorValue(String sensorId, int readId, int value);
        static void sendSensorValue(String sensorId, int readId, float value);

    private:
        static bool _initialized;
        static String _deviceId;
        static uint8_t _lastCommand;

        static void sendDeviceId();
        static void waitForConnection();
        static bool waitForPing(unsigned int timeoutMs);

        static StaticJsonDocument<1024> _lastJsonDocument;
        static uint8_t _messageCounter;
        static ReceivedMessage _receivedMessages[MAX_RECEIVED_MESSAGES];

        static bool internalReceive();

        static void initEvents();
        static uint8_t _eventCounter;
        static String _receivedEvents[MAX_RECEIVED_EVENTS];

        static uint8_t _servoCounter;
        static RegisteredServo _servos[MAX_REGISTERED_SERVO];
};
