from typing import Union
from dvbcdr.intercom import intercom_instance as intercom


def move_robot(robot_id, position, backwards=False):
    intercom.publish("robot_backwards", [robot_id, int(backwards)])

    if len(position) == 2:
        position = (*position, -1)

    intercom.publish("move_robot", [robot_id, *position])


def move_servo(servo_id: Union[str, int], angle: int) -> None:
    intercom.publish("api_request_servo" + str(servo_id), int(angle))


read_sensors = {}


def __received_sensor_value(read_id, sensor_value):
    read_sensors[read_id] = sensor_value
    intercom.unsubscribe(read_id)


def request_sensor_value(sensor_id: Union[str, int]) -> int:
    """
    Requests a value from a sensor. The type of value is determined by the implementation of the registered handler.

    WARNING: This does not return a value read from the sensor but an intercom callback id required to read the value.
    """
    read_id = intercom.subscribe("api_response_sensor" + str(sensor_id), lambda message: __received_sensor_value(int(message[0]), message[1]))
    intercom.publish("api_request_sensor" + str(sensor_id), read_id)
    return read_id


def access_sensor_value(read_id, autodelete=True):
    if read_id in read_sensors:
        value = read_sensors[read_id]
        if autodelete:
            del read_sensors[read_id]
        return value
