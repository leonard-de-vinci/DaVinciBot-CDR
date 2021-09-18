class Message:
    topic = None
    message_data = None

    def __init__(self, topic: str, message_data):
        self.topic = topic
        self.message_data = message_data