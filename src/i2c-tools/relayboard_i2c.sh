#!/bin/bash

I2C_BUS=0
I2C_ADDRESS=0x20

MCP_IODIR=0
MCP_OLAT=0xa

if ! i2cget -y ${I2C_BUS} ${I2C_ADDRESS} ${MCP_IODIR} > /dev/null; then
 echo "Can't find the relayboard at bus ${I2C_BUS} address ${I2C_ADDRESS}" >&2
 echo "Please verify the I2C_BUS and I2C_ADDRESS settings in the script $0" >&2
 exit 2
fi

i2cset -y ${I2C_BUS} ${I2C_ADDRESS} ${MCP_IODIR} 0x0 # IODIR=0

while true
do
 STATE=`i2cget -y ${I2C_BUS} ${I2C_ADDRESS} ${MCP_OLAT}`
 echo "Actual state of the relays: ${STATE}"
 pos=8
 value="${STATE}"
 for i in 128 64 32 16 8 4 2 1
 do
   echo -n "${pos}"
   pos=$(( pos - 1 ))
   bit=$(( value / i ))
   value=$(( value % i ))
   if [[ "$bit" -eq 0 ]]; then echo -n ": OFF "
   else echo -n ": ON  "
   fi
 done
 echo ""

 read -p 'Enter [12345678] toggle [C] clear all: ' KEY
 case "${KEY}" in
 [1-8])
   bit=$(( 2**(KEY-1) ))
   STATE=$(( STATE^bit ))
   ;;
 c|C)
   STATE=0
   ;;
 *)
   echo "Wrong key"
   continue
   ;;
 esac

 i2cset -y ${I2C_BUS} ${I2C_ADDRESS} ${MCP_OLAT} ${STATE}

done

