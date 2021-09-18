import threading
from typing import Callable, List, Union, Any
from .crc import *
from .callback import *
from .message import *

from threading import Thread
from queue import Queue
import socket

MULTICAST_TTL = 2

class Intercom:
    thread: threading.Thread = None

    com_socket: socket.socket = None
    send_queue = Queue()
    receive_queue = Queue()

    callbacks: List[Callback] = []

    def __intercom_thread(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        sock.setblocking(False)
        self.com_socket = sock

        while True:
            if not self.send_queue.empty():
                pass

            try:
                pass
            except BlockingIOError:
                # nothing to receive
                pass

    def start(self) -> None:
        """Starts the intercom thread. Automatically called by the first call to 'subscribe' or 'publish'."""
        if self.thread is not None:
            thread = Thread(target=self.__intercom_thread, daemon=True)
            thread.start()

    def subscribe(self, topics: Union[str, List[str]], action: Callable[[str, Any], None]) -> Callback:
        callback = Callback(topics, action)
        self.callbacks.append(callback)

        return callback

    def run_callbacks(self, process_limit=0):
        processed = 0
        while not self.receive_queue.empty():
            message = self.receive_queue.get()
            for callback in self.callbacks:
                if callback.topics is None or message.topic in callback.topics:
                    callback.run(message.topic, message.message_data)

            processed += 1
            if process_limit > 0 and processed >= process_limit:
                break


    def send(self, topic: str, message: Message) -> None:
        pass