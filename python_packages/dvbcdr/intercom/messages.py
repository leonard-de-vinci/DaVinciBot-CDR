from typing import Any


class ReceivedMessage:
    """Represents a message received by the intercom."""

    topic_code: int = None
    message_data = None

    def __init__(self, topic_code, message_data):
        """Creates a `ReceivedMessage` object from a topic code and a message data.

        Args:
            topic_code: The topic represented as its CRC24 code.
            message_data: Anything representing the received message data.
        """
        self.topic_code = topic_code
        self.message_data = message_data


class Message:
    """Represents a message about to be sent."""

    topic: str = None
    message_data = None

    def __init__(self, topic: str, message_data: Any = None):
        """Creates a message about to be sent from a topic and a message data.

        Args:
            topic: A string representing the topic.
            message_data: Anything representing the data to send.
        """
        self.topic = topic
        self.message_data = message_data
