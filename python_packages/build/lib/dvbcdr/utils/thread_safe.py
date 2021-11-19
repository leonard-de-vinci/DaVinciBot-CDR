from threading import Event, Lock
from typing import Any


class ThreadSafeDict(dict):
    def __init__(self, *args, **kwargs):
        dict.__init__(self, *args, **kwargs)

        self._lock = Lock()

    def __enter__(self):
        self._lock.acquire()
        return self

    def __exit__(self, ex_type, ex_value, ex_traceback):
        self._lock.release()


class ThreadSafeList(list):
    def __init__(self, *args, **kwargs):
        list.__init__(self, *args, **kwargs)

        self._lock = Lock()

    def __enter__(self):
        self._lock.acquire()
        return self

    def __exit__(self, ex_type, ex_value, ex_traceback):
        self._lock.release()


class DataEvent:
    __data = None

    def __init__(self):
        self.__event = Event()

    def unlock(self, data):
        self.__data = data
        self.__event.set()

    def wait(self) -> Any:
        self.__event.wait()
        return self.__data
