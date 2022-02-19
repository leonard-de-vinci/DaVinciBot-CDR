#ifdef ROBOT_0
#include <Arduino.h>
#include "robot.h"
#include "intercom.h"

String robot_name = "robot_0";

int excavation_known_resistor = 750; // ohms
int excavation_voltage_input = A9;

void robot_setup() {
    Intercom::registerSensor("excavation_read_sensor");

    pinMode(excavation_voltage_input, INPUT);
}

void robot_logic() {

}

void robot_loop() {
    int readId;

    if(Intercom::isSensorRequested("excavation_read_sensor", &readId)) {
        int value = analogRead(excavation_voltage_input);
        int resistor = (1024.0 / value - 1) * excavation_known_resistor;
        Intercom::sendSensorValue("excavation_read_sensor", readId, resistor);
    }
}
#endif