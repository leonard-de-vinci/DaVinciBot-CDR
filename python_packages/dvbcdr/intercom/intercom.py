import threading
import socket
import json
import struct
import sys
import warnings

from typing import Callable, Dict, List, Tuple, Union, Any
from threading import Condition, Event, Thread
from queue import Queue

from ..utils.thread_safe import DataEvent, ThreadSafeDict
from ..utils.crc import crc24
from .callback import Callback
from .messages import ReceivedMessage

MULTICAST_TTL = 2
MULTICAST_PORT = 5007
MULTICAST_BUFFSIZE = 4096

# always 4 bytes each
PACKET_START = b"\0DVB"
PACKET_END = b"\0CDR"


class Intercom:
    receive_thread: threading.Thread = None
    socket_ready = Event()

    com_socket: socket.socket = None
    receive_queue: "Queue[ReceivedMessage]" = Queue()

    callbacks: List[Callback] = []
    event_callbacks: List[Callback] = []
    wait_events: Dict[int, DataEvent] = ThreadSafeDict()
    __message_received = Condition()

    crc_cache: Dict[str, Tuple[int, str]] = {}
    """Stores crc code and ip for each subscribed topic."""

    def __topic_code_to_ip(self, code):
        topic_ip = "224"

        for _ in range(3):
            topic_ip += "." + str((code & 0xff0000) >> 16)
            code <<= 8

        return topic_ip

    def __get_topic_info(self, topic) -> Tuple[int, str]:
        """Internal method to retreive and cache the crc24 code and corresponding ip of a topic."""
        if not isinstance(topic, str):
            raise TypeError("topic needs to be a string")

        if topic not in self.crc_cache:
            topic_code = crc24(topic.lower())
            if topic_code == 0:
                warnings.warn("Topic '" + topic + "' results in a topic-code of 0 which is an invalid number, please use another topic name.", BytesWarning)

            topic_ip = self.__topic_code_to_ip(topic_code)

            self.crc_cache[topic] = (topic_code, topic_ip)

        return self.crc_cache[topic]

    def __intercom_thread(self):
        """
        Internal method used to setup the socket and receive messages.

        This method MUST NOT be run on the main thread as it is blocking the execution and never returns.
        """
        if threading.current_thread() is threading.main_thread():
            raise RuntimeError("__intercom_thread can't be run on the main thread!")

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        sock.bind(("", MULTICAST_PORT))

        self.com_socket = sock
        self.socket_ready.set()

        while True:
            try:
                received_bytes = sock.recv(MULTICAST_BUFFSIZE)
                if len(received_bytes) > (3 + 4 * 2) and received_bytes[:4] == PACKET_START and received_bytes[-4:] == PACKET_END:
                    topic_code = int.from_bytes(received_bytes[4:7], "big")
                    received_data = json.loads(received_bytes[7:-4])

                    if topic_code in self.wait_events:
                        self.wait_events[topic_code].unlock(received_data)
                        del self.wait_events[topic_code]

                    self.receive_queue.put(ReceivedMessage(topic_code, received_data))

                    with self.__message_received:
                        self.__message_received.notify_all()

            except BlockingIOError:
                pass  # nothing to receive
            except Exception:
                print("Intercom thread error", sys.exc_info()[0])

    def start(self):
        """Starts the intercom receive thread. Automatically called by the first call to 'subscribe' or 'publish'."""
        if self.receive_thread is None:
            self.receive_thread = Thread(target=self.__intercom_thread, daemon=True)
            self.receive_thread.start()

            self.socket_ready.wait()

    def subscribe(self, topics: Union[str, List[str]], action: Callable[[Any], None] = lambda *args: None) -> int:
        """
        Registers a callback for one or multiple topics.

        Args:
            topics: A string or a list of strings representing the topics you want to subscribe to.
            action: The method that should be called when a message is received on one of the given topics.
                This method takes a single argument, the message data, and should not return anything.

        Returns:
            The id of the registered callback that should be used to remove that subscription.
        """
        self.start()

        if isinstance(topics, str):
            topics = [topics]
        elif not isinstance(topics, list):
            raise TypeError("topics is not a string or a list of strings")

        topics = [x for x in topics if isinstance(x, str)]
        if len(topics) == 0:
            raise ValueError("topics doesn't contain any string")

        topic_codes: List[int] = []

        for topic in topics:
            already_registered = topic in self.crc_cache

            topic_code, topic_ip = self.__get_topic_info(topic)

            topic_codes.append(topic_code)

            if not already_registered:
                self.com_socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, struct.pack("4sl", socket.inet_aton(topic_ip), socket.INADDR_ANY))

        callback = Callback(topic_codes, action)
        self.callbacks.append(callback)

        return callback.ref

    def subscribe_raw(self, topic_code: int, action: Callable[[Any], None] = lambda *args: None) -> int:
        """
        Registers a callback for a single topic.

        Args:
            topic_code: The crc24 code of the topic you want to subscribe to.
            action: The method that should be called when a message is received on the given topic.
                This method takes a single argument, the message data, and should not return anything.

        Returns:
            The id of the registered callback that should be used to remove that subscription.
        """
        self.start()

        topic_ip = self.__topic_code_to_ip(topic_code)
        self.com_socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, struct.pack("4sl", socket.inet_aton(topic_ip), socket.INADDR_ANY))

        callback = Callback([topic_code], action)
        self.callbacks.append(callback)

        return callback.ref

    def on_events(self, action: Callable[[str], None] = lambda *args: None) -> int:
        """
        Registers a callback for all received events.
        An event is a message without any additional data, just an action that happened for example.

        Args:
            action: The method that should be called when an event is received.
                It takes a single argument, the event name, and should not return anything.

        Returns:
            The id of the registered callback that should be used to remove that subscription.
        """
        self.start()
        callback = Callback([0], action)

        if len(self.event_callbacks) == 0:
            event_ip = self.__topic_code_to_ip(0)
            self.com_socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, struct.pack("4sl", socket.inet_aton(event_ip), socket.INADDR_ANY))

        self.event_callbacks.append(callback)

        # a negative ref indicates that this is an event callback
        return -callback.ref

    def on_event(self, event_name: str, action: Callable[[None], None] = lambda *args: None) -> int:
        """
        Registers a callback for a specific event.
        An event is a message without any additional data, just an action that happened for example.

        Args:
            event_name: The name of the event you want to subscribe to.
            action: The method that should be called when an event is received.
                It takes no arguments and should not return anything.

        Returns:
            The id of the registered callback that should be used to remove that subscription.
        """

        if not callable(action):
            return 0

        return self.on_events(lambda received_name: action() if received_name == event_name else None)

    def wait_for_topic(self, topic: str, autosubscribe=True):
        """
        Blocks the execution until a message from the given topic is received and returns this message.

        Args:
            topic: A string corresponding to the topic the intercom needs to wait for.
            autosubscribe: If the given topic was not 'subscribed' by a call to `intercom.Subscribe()`, it automatically
                call the method. Raises a ValueError if this is set to False and the topic is not subscribed. Defaults to True.

        Returns:
            The data of the first message received on the given topic after the call of this method.

        Raises:
            ValueError: The given topic is not subscribed and autosubscribe is set to False.
        """
        if topic not in self.crc_cache:
            if autosubscribe:
                self.subscribe(topic, lambda *args: None)
            else:
                raise ValueError("topic is not currently subscribed and autosubscribe is not True")

        topic_code = self.__get_topic_info(topic)[0]

        if topic_code not in self.wait_events:
            self.wait_events[topic_code] = DataEvent()

        return self.wait_events[topic_code].wait()

    def run_callbacks(self, process_limit=0):
        """
        Run on any thread the callbacks for the messages received on the intercom thread.

        Args:
            process_limit: Limits the numbers of messages to process, default is 0 wich indicates no limit.
        """
        processed = 0
        while not self.receive_queue.empty():
            message = self.receive_queue.get()
            if message.topic_code == 0 and isinstance(message.message_data, str):
                for callback in self.event_callbacks:
                    callback.run(message.message_data)
            else:
                for callback in self.callbacks:
                    if message.topic_code in callback.topic_codes:
                        callback.run(message.message_data)

            processed += 1
            if process_limit > 0 and processed >= process_limit:
                break

    def unsubscribe(self, ref: int) -> None:
        """
        Unsubscribes a callback from the intercom.

        If ref is negative, it will unsubscribe the event callback.
        The reference of an event callback is automatically set to a negative number.

        Args:
            ref: The id of the callback to unsubscribe.
        """
        if ref > 0:
            for callback in self.callbacks:
                if callback.ref == ref:
                    self.callbacks.remove(callback)
                    break
        else:
            for callback in self.event_callbacks:
                if callback.ref == -ref:
                    self.event_callbacks.remove(callback)
                    break

    def wait_here(self):
        """Blocks the current thread and run callbacks until the program closes."""
        self.run_callbacks()  # run waiting callbacks first
        while True:
            with self.__message_received:
                self.__message_received.wait()
                self.run_callbacks()

    def wait_in_new_thread(self) -> Thread:
        """Creates a new thread and run callbacks on it until the program closes."""
        new_thread = Thread(target=self.wait_here, daemon=True)
        new_thread.start()

        return new_thread

    def publish(self, topic: str, message_data: Any = None) -> None:
        """
        Publishes a message to all the subscribers of a topic.

        Args:
            topic: A string representing the topic.
            message_data: Anything representing the data to send.
        """
        self.start()

        topic_info = self.__get_topic_info(topic)
        self.com_socket.sendto(PACKET_START + topic_info[0].to_bytes(3, "big") + bytes(json.dumps(message_data, separators=(",", ":")), "utf-8") + PACKET_END, (topic_info[1], MULTICAST_PORT))

    def publish_raw(self, topic_code: int, message_data: Any = None) -> None:
        self.start()

        topic_ip = self.__topic_code_to_ip(topic_code)
        self.com_socket.sendto(PACKET_START + topic_code.to_bytes(3, "big") + bytes(json.dumps(message_data, separators=(",", ":")), "utf-8") + PACKET_END, (topic_ip, MULTICAST_PORT))

    def publish_event(self, event_name: str) -> None:
        self.start()
        self.publish_raw(0, event_name)


intercom_instance: Intercom = None


def get_intercom_instance():
    """
    Returns the instance of intercom.
    If no instance is created yet, it creates a new one.
    """
    global intercom_instance
    if intercom_instance is None:
        intercom_instance = Intercom()
    return intercom_instance
