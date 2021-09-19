import random
from typing import Any, Callable, List

class Callback:
    action: Callable[[Any], None] = None
    topic_codes: List[int] = None

    ref = random.getrandbits(32)

    def __init__(self, topic_codes: List[int], action: Callable[[Any], None]):
        if not callable(action):
            raise TypeError("action is not callable")

        if not isinstance(topic_codes, list):
            raise TypeError("topic_codes is not a list")

        topic_codes = [x for x in topic_codes if isinstance(x, int) and x < (1 << 23)]
        if len(topic_codes) == 0:
            raise ValueError("topic_codes doesn't contain any 24-bit integer")

        self.topic_codes = topic_codes
        self.action = action

    def run(self, message_data):
        if self.action is not None:
           self.action(message_data)
