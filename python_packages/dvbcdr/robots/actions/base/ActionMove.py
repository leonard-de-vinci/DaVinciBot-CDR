from ...action import Action
from ....api import api
from dvbcdr.intercom import intercom_instance as intercom


class ActionMove(Action):
    def __init__(self, robot_id, position, backwards=False):
        self.robot_id = robot_id
        self.position = position
        self.backwards = backwards
        self.done = False

        self.registered_ref = intercom.subscribe("robot_arrived", self.__robot_arrived)

    def __robot_arrived(self, robot_id) -> None:
        if self.robot_id == robot_id:
            self.done = True

    def start(self) -> None:
        api.move_robot(self.robot_id, self.position, self.backwards)

    def tick(self) -> bool:
        return self.done

    def finish(self) -> None:
        intercom.unsubscribe(self.registered_ref)
