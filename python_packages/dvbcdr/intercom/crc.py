from typing import Union

CRC_POLY = 0x864cfb
CRC_INIT = 0xb704ce
MSB_MASK = 1 << 23
CRC_MASK = ((MSB_MASK - 1) << 1) | 1

def crc24(input: Union[str, int, bytes]) -> int:
    input_type = type(input)
    if input_type is str:
        input = str.encode(input)
    elif input_type is int:
        input = bytes([input])
    elif input_type is not bytes:
        raise TypeError("Cannot compute CRC24 from input of type " + str(input_type))
    
    crc = CRC_INIT
    for byte in input:
        crc ^= (byte << 16)
        for i in range(8):
            crc <<= 1
            if crc & MSB_MASK:
                crc ^= CRC_POLY

    return crc & CRC_MASK

def get_ip(topic: str) -> str:
    if type(topic) is not str:
        raise TypeError("topic needs to be a string")

    encoded = crc24(topic.lower())
    ip = "224"

    for i in range(3):
        ip += "." + str((encoded & 0xff0000) >> 16)
        encoded <<= 8

    return ip