#!/usr/bin/env python3

"""
Patch a rom file with the given bytes at the given position
"""

import sys
import os

if len(sys.argv) < 2:
    print("Not enough filenames given")
    exit()

romname = sys.argv[1]
bs = bytearray(open(romname, 'rb').read())

for a in sys.argv[2:]:
    args = a.split(',')
    to = int(args[0],16)
    p = bytearray.fromhex(args[1])
    bs[to:to+len(p)] = p

# Write the patched bytes
rom = open(romname, 'wb')
rom.write(bs)

