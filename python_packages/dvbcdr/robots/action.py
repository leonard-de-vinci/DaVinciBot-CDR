from abc import ABC, abstractmethod
from typing import List


class Action(ABC):
    @abstractmethod
    def start(self) -> None:
        pass

    @abstractmethod
    def tick(self) -> bool:
        pass

    @abstractmethod
    def finish(self) -> None:
        pass
