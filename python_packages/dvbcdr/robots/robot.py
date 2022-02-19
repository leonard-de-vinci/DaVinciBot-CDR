from enum import Enum
from typing import Tuple


class RobotSide(Enum):
    """
    Represents the different teams the robot can be in.

    The internal string representation can be changed each year to represent the color of the team.
    """
    LEFT = "yellow"
    RIGHT = "violet"


class Robot:
    """
    Represents a robot on the table.

    It includes its team (or side), its unique id (integer starting from 0) and its current position
    (in millimeters; the 3rd element is the robot rotation in degrees).
    """

    id: int
    side: RobotSide
    position: Tuple[int, int, int]

    def __init__(self, id: int, side: RobotSide):
        self.id = id
        self.side = side
