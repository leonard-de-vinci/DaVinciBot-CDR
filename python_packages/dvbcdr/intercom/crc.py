from typing import Union

CRC_POLY = 0x864cfb
CRC_INIT = 0xb704ce
MSB_MASK = 1 << 23
CRC_MASK = ((MSB_MASK - 1) << 1) | 1

def crc24(data: Union[str, int, bytes]) -> int:
    if isinstance(data, str):
        data = str.encode(data)
    elif isinstance(data, int):
        data = bytes([data])
    elif not isinstance(data, bytes):
        raise TypeError("Cannot compute CRC24 from input of type " + str(data))
    
    crc = CRC_INIT
    for byte in data:
        crc ^= (byte << 16)
        for _ in range(8):
            crc <<= 1
            if crc & MSB_MASK:
                crc ^= CRC_POLY

    return crc & CRC_MASK

def get_ip(topic: str) -> str:
    if not isinstance(topic, str):
        raise TypeError("topic needs to be a string")

    encoded = crc24(topic.lower())
    ip = "224"

    for _ in range(3):
        ip += "." + str((encoded & 0xff0000) >> 16)
        encoded <<= 8

    return ip