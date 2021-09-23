from typing import Union

CRC_POLY = 0x864cfb
CRC_INIT = 0xb704ce
MSB_MASK = 1 << 23
CRC_MASK = ((MSB_MASK - 1) << 1) | 1

def crc24(data: Union[str, int, bytes]) -> int:
    """Computes the CRC24 code of an input data.
    
    Args:
        data: The data that should be computed. Can be an int, a string or a `bytes` object.
        
    Returns:
        A 3-bytes int corresponding to the CRC24 value of the input."""

    if isinstance(data, str):
        data = str.encode(data)
    elif isinstance(data, int):
        data = bytes([data])
    elif not isinstance(data, bytes):
        raise TypeError("Cannot compute CRC24 from input of type " + str(type(data)))

    crc = CRC_INIT
    for byte in data:
        crc ^= (byte << 16)
        for _ in range(8):
            crc <<= 1
            if crc & MSB_MASK:
                crc ^= CRC_POLY

    return crc & CRC_MASK
    