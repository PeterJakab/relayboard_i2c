#!/usr/bin/python2

import relayboard
r = relayboard.Relayboard(type='usb-8ch')
while True:
    print("olat = 0x%02x" % r.get_channels())
    c = int(raw_input('channel:'))
    v = str(raw_input('value:'))
    r.set_channel(c, v)

