from .message import *
import random
from typing import Any, Callable, List, Union

class Callback:
    action = None
    topics = None

    ref = random.getrandbits(4)

    def __init__(self, topics: Union[str, List[str], None], action: Callable[[str, Any], None]):
        if not callable(action):
            raise TypeError("action is not callable")
        
        topics_type = type(topics)
        if topics_type is str:
            topics = [topics]
        elif topics_type is not list and topics is not None:
            raise TypeError("topics is not a string, a list of strings or None")

        if topics is not None:
            topics = [x for x in topics if type(x) is str]
            if len(topics) == 0:
                raise ValueError("topics doesn't contain any string")

        self.topics = topics
        self.action = action

    def run(self, topic: str, message_data):
        if self.action is not None:
           self.action(topic=topic, message_data=message_data)