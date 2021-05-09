import usb

def usb_find(vendor, product, serial=''):
    busses = usb.busses()

    for bus in busses:
      devices = bus.devices
      for dev in devices:
        iserial = ""
        if dev.iSerialNumber != 0:
          handle = dev.open()
          try:
            iserial = handle.getString(dev.iSerialNumber, 64)
          except usb.USBError as e:
            # print "exception", e
            pass

        if dev.idVendor == vendor and dev.idProduct == product:
          if serial == '':
            return handle
          if iserial == serial:
            return handle
        usb.close(handle)

    return False

class usb_drv:
    """ usb relayboard driver """
    USB_VENDOR = 0x04d8
    USB_PRODUCT = 0xf7c0
    SETPORT = 0x54
    GETPORT = 0x53
    SETDIR = 0x55
    SETBIT = 0x56
    CLEARBIT = 0x57


    def __init__(self, serial='', vendor=USB_VENDOR, product=USB_PRODUCT, channels=8):
        self._serial = serial
        self._vendor = vendor
        self._product = product
        self._channels = channels
        self._handle = usb_find(vendor, product, serial)
        if self._handle == False:
            raise "no device found"
            return False
        self._handle.claimInterface(0)

    def set_channels(self, state):
        """set all channels to the defined state"""
        self._handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE, self.SETPORT, 8, state, 0, 100)

    def set_channel(self, channel, state):
        """set the selected channel On, Off or Toggle"""
        if channel < 1:
            raise Exception("invalid channel %r" % channel)
        if channel > self._channels:
            raise Exception("invalid channel %r" % channel)
        bitvalue = ( 1 << (channel - 1) )

        if state == 0:
            self._handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE, self.CLEARBIT, 8, bitvalue, 0, 100)
        elif state == 1:
            self._handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE, self.SETBIT, 8, bitvalue, 0, 100)
        elif state == 2:
            raise Exception("toggle not implemented")
        else:
            raise Exception("invalid state %r" % state)

    def get_channels(self):
        """get actual state of all channels"""
        buffer = bytearray(self._handle.controlMsg(usb.TYPE_VENDOR | usb.ENDPOINT_IN, self.GETPORT, 8, 0, 0, 100))
        return buffer[0]
