import random
from typing import Any, Callable, List


class Callback:
    """Represents a method associated with one or more intercom topics."""

    action: Callable[[Any], None] = None
    topic_codes: List[int] = None

    ref: int

    def __init__(self, topic_codes: List[int], action: Callable[[Any], None]):
        """
        Links a method to its corresponding subscribed topic codes.

        Args:
            topic_codes: A list of int, the CRC24 codes of the topics that callback should be executed for.
            action: The method that should be called. Takes a single argument, the message data and should not return anything.
        """
        if not callable(action):
            raise TypeError("action is not callable")

        if not isinstance(topic_codes, list):
            raise TypeError("topic_codes is not a list")

        topic_codes = [x for x in topic_codes if isinstance(x, int) and x < (1 << 23)]
        if len(topic_codes) == 0:
            raise ValueError("topic_codes doesn't contain any 24-bit integer")

        self.topic_codes = topic_codes
        self.action = action
        self.ref = random.getrandbits(31)

    def run(self, message_data):
        """
        Runs this callback action with a message_data.

        Args:
            message_data: The received message data that should be given to the action.
        """
        if self.action is not None:
            self.action(message_data)
