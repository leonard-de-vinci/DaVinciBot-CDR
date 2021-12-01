#include <Arduino.h>
#include <ArduinoJson.h>

class Intercom {
    public: 
        static void init(String deviceId, unsigned long speed);

        static void publish(String topic, int value);
        static void publish(String topic, String value);
        static void publish(String topic, float value);
        static void publish(String topic, double value);

        static void publish(String topic, int* ptr, int length);
        static void publish(String topic, String* ptr, int length);
        static void publish(String topic, float* ptr, int length);
        static void publish(String topic, double* ptr, int length);
 
        static void subscribe(String topic);

        // only works for subscribed topics
        static int receiveInt();
        static String receiveString();
        static float receiveFloat();
        static double receiveDouble();
        static int* receiveIntArray();
        static String* receiveStringArray();
        static float* receiveFloatArray();
        static double* receiveDoubleArray();
    
        static int receiveInt(String topic);
        static String receiveString(String topic);
        static float receiveFloat(String topic);
        static double receiveDouble(String topic);
        static int* receiveIntArray(String topic);
        static String* receiveStringArray(String topic);
        static float* receiveFloatArray(String topic);
        static double* receiveDoubleArray(String topic);

        static bool instantReceiveInt(String* topic, int* value);
        static bool instantReceiveString(String* topic, String* value);
        static bool instantReceiveFloat(String* topic, float* value);
        static bool instantReceiveDouble(String* topic, double* value);
        static bool instantReceiveIntArray(String* topic, int* value, int* length);
        static bool instantReceiveStringArray(String* topic, String* value, int* length);
        static bool instantReceiveFloatArray(String* topic, float* value, int* length);
        static bool instantReceiveDoubleArray(String* topic, double* value, int* length);

    private:
        static bool _initialized;
        static String _deviceId;

        static void sendDeviceId();
        static void waitForConnection();
        static bool waitForPing(unsigned int timeoutMs);

        static StaticJsonDocument<256> _jsonDocument;
        static uint8_t _lastCommand;
        static uint8_t _lastType;
        static uint32_t _lastTopic;
        static int8_t _lastLength;
        static void internalReceive();
};
