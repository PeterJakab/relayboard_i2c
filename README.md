This is the support package for the I2C relay board module
Please check http://jap.hu/el/relay_module.html for the details

├── bin
│   ├── linux.x86
│   │   └── relayctl - relay control utility for 32-bit Linux
│   └── raspberry
│       └── relayctl - relay control utility for the Raspberry Pi
└── src
    ├── arduino
    │   └── relayboard_i2c.ino - example source code to control the relay board from an Arduino
    ├── i2c-tools
    │   └── relayboard_i2c.sh - example shell code to control the relay board by Linux i2c-tools
    └── relayctl
        ├── relayctl.c - C source code for the relay control utility
        └── relayctl.h

