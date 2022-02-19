from abc import ABC, abstractmethod
from typing import List


class Action(ABC):
    @abstractmethod
    def start(self) -> None:
        """
        Method only ran once when the action is started (ie. to register callbacks).
        """
        pass

    @abstractmethod
    def tick(self, ticks) -> bool:
        """
        Method ran every tick (in loop) by the mission (and indirectly be the robot manager).

        Args:
            ticks: The number of ticks since this action was started.

        Returns:
            True if this action is finished, False otherwise.
        """
        pass

    @abstractmethod
    def finish(self) -> None:
        """
        Method only ran once when the action is finished to free resources.
        """
        pass
