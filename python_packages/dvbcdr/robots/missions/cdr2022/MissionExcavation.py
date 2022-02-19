from ...robot import RobotSide
from .. import Mission
from ...actions import ActionMove, ActionMeasure, ActionServo


class MissionExcavation(Mission):
    resistor_tolerance = 50
    valid_resistors = {
        RobotSide.LEFT: 1000,
        RobotSide.RIGHT: 470
    }

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.add_actions(self.__get_move_action(0),
                         *self.__get_measure_actions(0),
                         # custom logic until id3 will be added by the first measure
                         *self.__get_flip_actions(),
                         self.__get_move_action(3),
                         *self.__get_measure_actions(3),
                         # custom logic will be added by the 2nd measure
                         *self.__get_flip_actions())

        # MoveTo0
        # MoveServoDown
        # Measure
        # MoveServoUp
        # ok  -> Flip, MoveTo1
        # not -> MoveTo1, Flip, MoveTo2
        # Flip
        # MoveTo3
        # MoveServoDown
        # Measure
        # MoveServoUp
        # ok  -> Flip, MoveTo6
        # not -> MoveTo4, Flip, MoveTo5
        # Flip

    def __square_position(self, square_id: int) -> None:
        local_pos = 87 + square_id * 175
        return (625 + local_pos if self.robot.side == RobotSide.LEFT else 2375 - local_pos, 1880)

    def __get_measure_actions(self, square_id: int):
        return [ActionServo("excavation_read_servo", 90),
                ActionMeasure("excavation_read_sensor", lambda resistor: self.__resistor_received(square_id, resistor)),
                ActionServo("excavation_read_servo", 0)]

    def __get_flip_actions(self):
        return [ActionServo("excavation_return_servo", 90, delay=1),
                ActionServo("excavation_return_servo", 0, delay=1)]

    def __get_move_action(self, square_id: int):
        return ActionMove(self.robot.id, self.__square_position(square_id), self.robot.side == RobotSide.RIGHT)

    def __resistor_received(self, square_id: int, resistor: int):
        if square_id != 0 and square_id != 3:
            return

        check = self.__check_resistor(resistor)

        if square_id == 0:
            if check:
                self.add_next_actions(*self.__get_flip_actions(),
                                      self.__get_move_action(1))
            else:
                self.add_next_actions(self.__get_move_action(1),
                                      *self.__get_flip_actions(),
                                      self.__get_move_action(2))
        else:
            if check:
                self.add_next_actions(*self.__get_flip_actions(),
                                      self.__get_move_action(6))
            else:
                self.add_next_actions(self.__get_move_action(4),
                                      *self.__get_flip_actions(),
                                      self.__get_move_action(5))

    def __check_resistor(self, resistor: int):
        side = self.robot.side

        if side in self.valid_resistors:
            valid = self.valid_resistors[side]
            return valid - self.resistor_tolerance <= resistor <= valid + self.resistor_tolerance
        else:
            return False

    def get_name(self):
        return "FlipExcavationSquares"

    def get_points(self):
        return 25

    def required_delay(self):
        return 30

    def required_robots(self):
        return [0]
