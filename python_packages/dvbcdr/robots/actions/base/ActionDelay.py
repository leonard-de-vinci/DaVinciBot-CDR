import time
from ...action import Action


class ActionDelay(Action):
    def __init__(self, delay: float):
        self.delay = delay

    def start(self) -> None:
        self.start_time = time.time()

    def tick(self) -> bool:
        return time.time() - self.start_time > self.delay
