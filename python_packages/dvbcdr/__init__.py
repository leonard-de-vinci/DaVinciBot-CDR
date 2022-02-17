"""dvbcdr
======
DaVinciBot python modules for the French Robotics Cup (CDR)

Available modules
-----------------
utils
    Custom classes and methods (crypto, math, time) for other modules.
intercom
    Communication between devices, processes using UDP multicast.
"""

from .intercom import *
from . import utils
