import time
from .. import Action


class ActionDelay(Action):
    """
    Action that pauses the flow of a mission for a given amount of time.
    """

    def __init__(self, delay: float):
        """
        Creates an action to pause the action flow of a mission for a given amount of time.

        Args:
            delay: The amount of time to pause in seconds.
        """
        self.delay = delay

    def start(self) -> None:
        self.start_time = time.time()

    def tick(self, ticks) -> bool:
        return time.time() - self.start_time > self.delay
