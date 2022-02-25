#include <Arduino.h>
#include <FastLED.h>
#include <PWMServo.h>
#include "constants.h"

bool detected = true;
CRGB leds[NUM_LEDS];

PWMServo dinoServo;

void setup() {
    pinMode(PIN_SENSOR, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    
    dinoServo.attach(PIN_SERVO);
    FastLED.addLeds<NEOPIXEL, 4>(leds, NUM_LEDS);
}

int state = 0;
bool servoState = false;

void loop() {
    if(digitalRead(PIN_SENSOR) == LOW) {
        detected = true;
    }

    if(detected) {
        delay(300);
        for(int i = 0; i < NUM_LEDS; i++) {
            int pos = (i + state) % NUM_LEDS;
            leds[pos] = (i/2) % 2 == 0 ? CRGB::Purple : CRGB::Yellow;
        }

        FastLED.show();

        if(state == 0) {
            if(servoState) {
                dinoServo.write(0);
                digitalWrite(LED_BUILTIN, LOW);
            } else {
                dinoServo.write(180);
                digitalWrite(LED_BUILTIN, HIGH);
            }

            servoState = !servoState;
        }

        state = (state + 1) % 4;
    }
}