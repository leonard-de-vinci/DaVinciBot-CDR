from typing import Union
from .. import ActionDelay
from .... import api


class ActionServo(ActionDelay):
    """
    Action that moves a servo to a given angle.
    """

    def __init__(self, servo_id: Union[str, int], angle: int, delay: float = 0.5):
        """
        Creates an action to move a servo to a given angle.

        Args:
            servo_id: The id/name of the servo to move.
            angle: The angle to move the servo to in degrees.
            delay: The amount of time to pause in seconds for the servo to reach the desired angle. Default value is 0.5 seconds.
        """
        super().__init__(delay)

        self.servo_id = servo_id
        self.angle = angle

    def start(self):
        super().start()
        api.move_servo(self.servo_id, self.angle)
