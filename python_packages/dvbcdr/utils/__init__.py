"""dvbcdr.utils
===============

Utilities used in the dvbcdr package, including crypto and maths functions and custom classes.
"""

from .crc import crc24
from .thread_safe import ThreadSafeDict, DataEvent
from .benchmark import gen_numpy_set, numpy_load, gen_python_set, python_load
from .maths import round_significant_decimals
