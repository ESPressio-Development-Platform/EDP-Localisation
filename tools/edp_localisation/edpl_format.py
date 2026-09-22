from __future__ import annotations

from .common import EDPL_FORMAT_MAJOR, EDPL_FORMAT_MINOR

SECTION_LANGUAGE_METADATA = 1
SECTION_LANGUAGE_DISPLAY_NAMES = 2
SECTION_GENERAL_STRINGS = 3
SECTION_TYPE_SCHEMA = 4
SECTION_UTF8_PAYLOAD = 5
REQUIRED_SECTIONS = (
    SECTION_LANGUAGE_METADATA, SECTION_LANGUAGE_DISPLAY_NAMES, SECTION_GENERAL_STRINGS,
    SECTION_TYPE_SCHEMA, SECTION_UTF8_PAYLOAD,
)
PREAMBLE_SIZE = 22
DIRECTORY_ENTRY_SIZE = 12
HEADER_SIZE = PREAMBLE_SIZE + len(REQUIRED_SECTIONS) * DIRECTORY_ENTRY_SIZE
SECTION_VERSION = 1
TERMINAL_FLAG = 0x01
NAME_PRESENT = 0x01
DESCRIPTION_PRESENT = 0x02
CRC_OFFSET = 18
CRC_SIZE = 4


def crc32c(data: bytes, zero_range: tuple[int, int] | None = None) -> int:
    crc = 0xFFFFFFFF
    zero_start, zero_length = zero_range if zero_range is not None else (-1, 0)
    zero_end = zero_start + zero_length
    for index, raw in enumerate(data):
        byte = 0 if zero_start <= index < zero_end else raw
        crc ^= byte
        for _ in range(8):
            crc = ((crc >> 1) ^ 0x82F63B78) & 0xFFFFFFFF if (crc & 1) else (crc >> 1)
    return crc ^ 0xFFFFFFFF
