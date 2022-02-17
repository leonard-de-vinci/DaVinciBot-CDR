from enum import Enum


class RobotSide(Enum):
    LEFT = "yellow"
    RIGHT = "violet"


class Robot:
    side: RobotSide

    def __init__(self, id: int, side: RobotSide):
        self.id = id
        self.side = side
