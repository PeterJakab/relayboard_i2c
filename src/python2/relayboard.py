import Driver.i2c
import Driver.usb_drv

class Relayboard:
    """Relayboard - universal relay board class """

    def __init__(self, type, *args, **kwargs):
        self._type = type
        if type == "i2c-8ch":
            self._channels = 8
            self._driver = Driver.i2c.i2c(*args, channels=self._channels, **kwargs)
        elif type == "usb-8ch":
            self._channels = 8
            self._driver = Driver.usb_drv.usb_drv(*args, channels=self._channels, **kwargs)
        else:
            raise "invalid Relayboard type", type

    def set_channels(self, state):
        """set all channels to the defined state"""
        self._driver.set_channels(state)

    def set_channel(self, channel, state):
        """set the selected channel On, Off or Toggle"""

        if channel < 1:
            raise "invalid channel", channel
        if channel > self._channels:
            raise "invalid channel", channel

        if isinstance(state, str):
            if state == 'On':
                self._driver.set_channel(channel, 1)
            elif state == 'Off':
                self._driver.set_channel(channel, 0)
            elif state == 'Toggle':
                self._driver.set_channel(channel, 2)
            else:
                raise "invalid state", state

        elif isinstance(state, bool):
            if state == True:
                self._driver.set_channel(channel, 1)
            else:
                self._driver.set_channel(channel, 0)

        elif isinstance(state, int):
            self._driver.set_channel(channel, state)

        else:
            raise "invalid state", state

    def set_channel_on(self, channel):
        """turn the selected channel on"""
        self._driver.set_channel(channel, 1)

    def set_channel_off(self, channel):
        """turn the selected channel off"""
        self._driver.set_channel(channel, 0)

    def set_channel_toggle(self, channel):
        """toggle the selected channel on or off"""
        self._driver.set_channel(channel, 2)

    def get_channels(self):
        """get actual state of all channels"""
        return self._driver.get_channels()
