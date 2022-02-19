from typing import Union
from .. import Action
from .... import api


class ActionMeasure(Action):
    """
    Action that measures a sensor value and optionally calls a callback function to allow conditional execution of the mission.
    """

    def __init__(self, sensor_id: Union[str, int], callback: Action[[Union[int, float]], None] = None):
        """
        Creates an action to measure a sensor value.

        It finishes when the value is received from the sensor. This value will be stored in the `value` attribute
        and passed to the callback function if provided to allow conditional execution of the mission by adding
        new actions after this one based on the measured value with `mission.add_next_actions()`.

        Args:
            sensor_id: The id/name of the sensor to read a value from.
            callback: The optional callback function to be called when the value is received.
        """
        self.sensor_id = sensor_id
        self.callback = callback

        self.read_ref = -1
        self.value = None

    def start(self) -> None:
        self.read_ref = api.request_sensor_value(self.sensor_id)

    def tick(self, ticks) -> bool:
        if self.read_ref > 0:
            value = api.access_sensor_value(self.read_ref)
            if value is not None:
                self.value = value
                if self.callback is not None:
                    self.callback(value)
                return True

        return False
