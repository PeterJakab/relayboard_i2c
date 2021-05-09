#!/usr/bin/python3

import relayboard
r = relayboard.Relayboard(type='usb-8ch')
while True:
    print("olat = 0x%02x" % r.get_channels())
    c = int(input('channel:'))
    v = str(input('value:'))
    r.set_channel(c, v)

