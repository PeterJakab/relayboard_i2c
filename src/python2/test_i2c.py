#!/usr/bin/python2

import relayboard
r = relayboard.Relayboard(type='i2c-8ch', bus=1, address=0x20)
while True:
    print("olat = 0x%02x" % r.get_channels())
    c = int(raw_input('channel (1-8):'))
    v = str(raw_input('value (On, Off, Toggle):'))
    r.set_channel(c, v)

