from typing import Union, Tuple
from .. import Action
from .... import api
from ....intercom import intercom_instance as intercom


class ActionMove(Action):
    """
    Action that moves a robot to a given position.
    """

    def __init__(self, robot_id: int, position: Union[Tuple[int, int], Tuple[int, int, int]], backwards=False):
        """
        Creates an action to move a robot to a given position on the game table.

        This action finishes when the robot has arrived to its destination.

        Args:
            robot_id: An integer representing the id of the robot to move.
            position: A tuple of integers representing the position in millimeters to move the robot to.
                If the tuple has 3 elements, it will indicate the rotation (in degrees) the robot should be in after arriving.
            backwards: If True, the robot will move backwards. Default value is False.
        """
        self.robot_id = robot_id
        self.position = position
        self.backwards = backwards
        self.registered_ref = -1

        self.done = False

    def start(self) -> None:
        self.registered_ref = intercom.subscribe("robot_arrived", self.__robot_arrived)

    def __robot_arrived(self, robot_id) -> None:
        """
        Internal callback used by intercom when the robot arrives at its destination.
        """
        if self.robot_id == robot_id:
            self.done = True

    def tick(self, ticks) -> bool:
        if ticks % 100 == 0:
            # send the messages multiple times in case of the Teensy loses the requested position
            api.move_robot(self.robot_id, self.position, self.backwards)

        return self.done  # or ticks > 100  # debug :)

    def finish(self) -> None:
        if self.registered_ref > 0:
            intercom.unsubscribe(self.registered_ref)
