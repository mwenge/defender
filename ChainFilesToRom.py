#!/usr/bin/env python3

"""
Chain together the relevant parts of the compiled binaries into a complete defender rom
"""

import sys
import os

if len(sys.argv) < 2:
    print("Not enough filenames given")
    exit()

romname = sys.argv[1]
rom = open(romname, 'wb')

romlength = int(sys.argv[2],16)

rom_data = bytearray([0xff] * romlength)
for a in sys.argv[3:]:
    args = a.split(',')
    fn = args[0]
    nm = args[-1:]
    fr,to,ln = [int(x,16) for x in args[1:-1]]
    
    bs = open(fn, 'rb').read()[fr-1:fr+ln-1]
    rom_data[to:to+len(bs)] = bs
rom.write(rom_data)
