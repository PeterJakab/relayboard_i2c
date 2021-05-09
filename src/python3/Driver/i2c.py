import smbus

class i2c:
    """ i2c relayboard driver """
    MCP_IODIR=0
    MCP_OLAT=0x0a

    def _get_olat(self):
        if self._cache == False:
            self._olat = self._conn.read_byte_data(self._address, self.MCP_OLAT) # MCP_OLAT

    def __init__(self, bus=0, address=0x20, channels=8, cache=False):
        self._bus = bus
        self._address = address
        self._channels = channels
        self._conn = smbus.SMBus(self._bus)
        self._conn.write_byte_data(self._address, self.MCP_IODIR, 0) # MCP_IODIR
        self._cache = False
        self._get_olat()
        self._cache = cache

    def set_channels(self, state):
        """set all channels to the defined state"""
        self._conn.write_byte_data(self._address, self.MCP_OLAT, state)

    def set_channel(self, channel, state):
        """set the selected channel On, Off or Toggle"""
        if channel < 1:
            raise Exception("invalid channel %r" % channel)
        if channel > self._channels:
            raise Exception("invalid channel %r" % channel)
        bitvalue = ( 1 << (channel - 1) )
        self._get_olat()

        if state == 0:
            self._olat &= (~bitvalue)
        elif state == 1:
            self._olat |= bitvalue
        elif state == 2:
            self._olat ^= bitvalue
        else:
            raise Exception("invalid state %r" % state)

        self._conn.write_byte_data(self._address, self.MCP_OLAT, self._olat)

    def get_channels(self):
        """get actual state of all channels"""
        self._get_olat()
        return self._olat
