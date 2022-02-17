from abc import abstractmethod
from typing import List, Tuple

from python_packages.dvbcdr.robots.robot import Robot
from .action import Action


class Mission(Action):
    current_action_id = 0
    actions: List[Action] = []

    def __init__(self, robot: Robot):
        self.robot = robot

    @abstractmethod
    def get_name(self) -> str:
        pass

    @abstractmethod
    def get_points(self) -> int:
        pass

    @abstractmethod
    def required_delay(self, irl_position: Tuple[int, int, int]) -> int:
        pass

    @abstractmethod
    def required_robots(self) -> List[int]:
        return []

    def tick(self) -> bool:
        if self.current_action_id < len(self.actions):
            if self.actions[self.current_action_id].tick():
                self.actions[self.current_action_id].finish()
                self.current_action_id += 1
                self.actions[self.current_action_id].start()
            return self.current_action_id >= len(self.actions)
        else:
            return True

    def start(self) -> None:
        self.current_action_id = 0
        self.actions[0].start()

    def finish(self) -> None:
        if len(self.actions) > self.current_action_id:
            self.actions[self.current_action_id].finish()

    def add_actions(self, *args: Action) -> None:
        for action in args:
            self.actions.append(action)

    def add_next_actions(self, *args: Action, margin=0) -> None:
        for i, action in enumerate(args, 1):
            self.actions.insert(self.current_action_id + margin + i, action)
