from ...robot import RobotSide
from .. import Mission
from ...actions import ActionMove, ActionServo


class MissionWorkshed(Mission):
    positions = [
        (800, 1025),
        (1150, 1025),
        (1150, 1200),
        (800, 1520),
        (430, 1520),
        (280, 1710)
    ]

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.add_actions(self.__get_move_action(0),
                         self.__get_move_action(1),
                         *self.__get_down_actions(),
                         self.__get_move_action(2),
                         self.__get_move_action(3),
                         self.__get_move_action(4),
                         self.__get_move_action(5),
                         ActionMove(self.robot.id, self.__get_move_action(4).position, backwards=True),
                         *self.__get_up_actions())

    def __get_move_action(self, index):
        position = self.positions[index]
        return ActionMove(self.robot.id, (position[0] if self.robot.side == RobotSide.LEFT else 3000 - position[0], position[1]))

    def __get_down_actions(self):
        return [ActionServo("workshed_arm_left", 90, delay=1),
                ActionServo("workshed_arm_right", 90, delay=1)]

    def __get_up_actions(self):
        return [ActionServo("workshed_arm_left", 0, delay=1),
                ActionServo("workshed_arm_right", 0, delay=1)]

    def get_name(self):
        return "StoreSamplesInWorkshed"

    def get_points(self):
        return 15  # + 10 if more arms to collect the samples above the workshed

    def required_delay(self):
        return 20

    def required_robots(self):
        return [0]
