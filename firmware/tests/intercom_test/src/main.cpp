#include <Arduino.h>
#include <intercom.h>

void setup() {
  Intercom::init("test", 115200);
}

void loop() {
  Intercom::publish("test_topic", "test_message");
  delay(1000);
  Intercom::publish("topic_number", 10);
  delay(1000);
}