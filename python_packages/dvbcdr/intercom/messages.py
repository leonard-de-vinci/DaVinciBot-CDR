class ReceivedMessage:
    topic_code: int = None
    message_data = None

    def __init__(self, topic_code, message_data):
        self.topic_code = topic_code
        self.message_data = message_data

class Message:
    topic: str = None
    message_data = None

    def __init__(self, topic: str, message_data):
        self.topic = topic
        self.message_data = message_data

