"""dvbcdr.intercom
===============

Intercom module for the French Robotics Cup.
Works with multiple devices on the same LAN with UDP multicast.
"""

from .intercom import Intercom
from .callback import Callback
from .messages import ReceivedMessage
