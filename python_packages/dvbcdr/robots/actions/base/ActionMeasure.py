from typing import Any, Union
from ...action import Action
from ....api import api


class ActionMeasure(Action):
    def __init__(self, sensor_id: Union[str, int], callback: Action[[Any], None] = None):
        self.sensor_id = sensor_id
        self.callback = callback
        self.read_ref = -1

    def start(self) -> None:
        self.read_ref = api.request_sensor_value(self.sensor_id)

    def tick(self) -> bool:
        if self.read_ref > 0:
            value = api.access_sensor_value(self.read_ref)
            if value is not None:
                if self.callback is not None:
                    self.callback(value)
                return True

        return False
