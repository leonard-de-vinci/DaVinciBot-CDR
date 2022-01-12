Folder containing python packages for the robot, installable using (from this folder) `pip install -U ./`

# Intercom quick-start

The main package used by this project is *intercom*. It doesn't require any server as it run with *UDP multicast sockets* between all clients of a *LAN*.

It works by sending *data* to a *topic*. Data can be anything, as long as clients can decode it. By using the Python library, it means anything serializable to json string.

Internally, each topic is stored as a 24-bit integer computed using the *CRC24* algorithm (derived from the well-known *CRC32*). This integer is then split into 3 bytes and appended to 224 to create an IP address in the range `224.0.0.0/8`.

**Warning:** You only receive messages if you subscribed to that topic.


### Quick example:

*All packages include in-code comments to be used with any IntelliSense or autocompletion software.*

Start by importing the `Intercom` class and create an intercom client by instancing this class:
```python
from dvbcdr.intercom import Intercom
intercom = new Intercom()
```

To subscribe to a topic, use `subscribe`. It accepts a string or a list of strings as the topic(s) you want to subscribe to and a callback method:
```python
def callback(data):
    print(data)

intercom.subscribe('topic1', callback)
intercom.subscribe(['topic2', 'topic3'], callback)
```

As Python is a *mainly single-threaded* language, you must ask the library to run your callbacks:
```python
intercom.run_callbacks()
```

However, this runs the callbacks once then continues. You can ask the library to run the callbacks indefinitely and block your current thread or be starting a new thread to do that:
```python
intercom.wait_here()
# or
intercom.wait_in_new_thread()
```

Finally, to publish data to a topic, just use the `publish` method with a topic and some data:
```python
intercom.publish('topic1', 'Hello world!')
```

# Utils overview

The `dvbcdr` package also contains utilities to compute *CRC24* hashes or extend classes to be *thread safe*.

### `crc.py`
```python
def crc24(data: Union[str, int, bytes]) -> int
# computes the crc24 of its argument
```

### `maths.py`
```python
def round_significant_decimals(number: Union[float, int], significant_decimals: int = 3) -> Union[float, int]
# rounds a number to a given number of significant decimals
```

### `thread_safe.py`
```python
class ThreadSafeDict(dict)
# a thread safe dictionary (same features a normal dict but with thread safety)

class ThreadSafeList(list)
# same, a normal Python list extended with thread satety

class DataEvent
# replacement for the default 'Event' class that can carry data
```

### `time.py`
```python	
timer = new RepeatedTimer(1, print, 'hello')
# creates a RepeatedTimer instances that runs the print method every second with the argument 'hello'

timer.start()
# starts the loop, executing the callback forever

timer.stop()
# stops the loop, the callback will not be executed anymore