#!/usr/bin/python3

import relayboard
r = relayboard.Relayboard(type='i2c-8ch', bus=1, address=0x20)
while True:
    print("olat = 0x%02x" % r.get_channels())
    c = int(input('channel (1-8):'))
    v = str(input('value (On, Off, Toggle):'))
    r.set_channel(c, v)

