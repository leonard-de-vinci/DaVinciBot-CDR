from typing import Tuple
from .ActionDelay import ActionDelay
from ....api import api


class ActionServo(ActionDelay):
    def __init__(self, servo_id: Tuple[str, int], angle: int, delay: float = 0.5):
        super().__init__(delay)

        self.servo_id = servo_id
        self.angle = angle

    def start(self):
        super().start()
        api.move_servo(self.servo_id, self.angle)
