"""dvbcdr.intercom
===============

Intercom module for the French Robotics Cup.
Works with multiple devices on the same LAN with UDP multicast.
"""

from .intercom import Intercom, get_intercom_instance
from .callback import Callback
from .messages import ReceivedMessage

intercom_instance = get_intercom_instance()
