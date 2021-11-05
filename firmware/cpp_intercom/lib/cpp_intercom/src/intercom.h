#include <Arduino.h>
#include <ArduinoJson.h>

class Intercom {
    public: 
        static void init(String deviceId, int speed = 115200);

        static void publish(String topic, int value);
        static void publish(String topic, String value);
        static void publish(String topic, float value);
 
        static void subscribe(String topic);

        // only works for subscribed topics
        static int receiveInt();
        static String receiveString();
        static float receiveFloat();
    
        static int receiveInt(String topic);
        static String receiveString(String topic);
        static float receiveFloat(String topic);

    private:
        static bool _initialized;
        inline static String _deviceId;

        static void sendDeviceId();
        static void waitForConnection();
        static bool waitForPing(int timeoutMs);

        inline static StaticJsonDocument<256> _jsonDocument;
        inline static uint8_t _lastCommand;
        inline static uint8_t _lastType;
        inline static uint32_t _lastTopic;
        static void internalReceive();
};