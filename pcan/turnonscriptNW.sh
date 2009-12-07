#!/bin/sh
# $Id$

# load the TCPU and TDIG FPGA from Eeprom 2
./pc "m s 0x7f2 5 0x8a 0x69 0x96 0xa5 0x5a 251"
usleep 500000
./pc "m e 0x1fc8007f 5 0x8a 0x69 0x96 0xa5 0x5a 251"
usleep 1300000

# Start detector West
./pc "m s 0x202 5 0x8a 0x69 0x96 0xa5 0x5a 252"
usleep 70000
./pc "m e 0x1fc80020 5 0x8a 0x69 0x96 0xa5 0x5a 252"
usleep 70000

# turn off all of the Serdes channels on THUB
for ((i=91; i<99; i+=1)) do
    ./pc "m s 0x402 2 0x"$i" 0x0 251"
    usleep 70000
done

# reset all of the TDCs on each TDIG
#for ((i=21; i<40; i+=1)); do
#    for j in 40 44 48 4c 50 54 58 5c; do
#	./pc "m e 0x"$j"800"$i" 1 0x90 251"
#	./pc "m e 0x"$j"800"$i" 5 0xe 0x2 0x1 0x2 0x0 251"
#	usleep 70000
#    done
#done

#for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
#    for j in 40 44 48 4c 50 54 58 5c; do
#	./pc "m e 0x"$j"800"$i" 1 0x90 251"
#	./pc "m e 0x"$j"800"$i" 5 0xe 0x2 0x1 0x2 0x0 251"
#	usleep 70000
#    done
#done

# Start detector West
#for j in 40 44 48 50 54; do
#    ./pc "m e 0x"$j"80020 1 0x90 252"
#    ./pc "m e 0x"$j"80020 5 0xe 0x2 0x1 0x2 0x0 252"
#    usleep 70000
#done

# set threshold on each TDIG board to 1200mV
./xsetThreshold 251 0x7f 0x7f 1200
usleep 1500000
# put TCPUs into run mode and turn on Serdes, turn off CANbus data
./pc "m s 0x7f2 3 0xe 0x2 0xf 251"
usleep 800000
# set mult gate delay
./pc "m s 0x7f2 3 0xe 0x8 0xe0 251"
usleep 700000


# start detector West: set threshold on each TDIG to 500mV
./xsetThreshold 252 0x7f 0x20 500
sleep 1
# start detector West: put TCPUs into run mode and turn on Serdes, turn off CANbus data
./pc "m s 0x202 3 0xe 0x2 0xf 252"
usleep 70000

# turn on L0 test pattern on tray 47W
#./pc "m s 0x3b2 3 0xe 0x9 0x80 251"
#usleep 70000

# turn on THUB serdes channels and put into regular trigger mode
./pc "m s 0x402 2 0x91 0x1f 251"
usleep 70000
./pc "m s 0x402 2 0x92 0x1f 251"
usleep 70000
./pc "m s 0x402 2 0x93 0x1f 251"
usleep 70000
./pc "m s 0x402 2 0x94 0x1f 251"
usleep 70000
./pc "m s 0x402 2 0x95 0x1f 251"
usleep 70000
./pc "m s 0x402 2 0x96 0x1f 251"
usleep 70000
./pc "m s 0x402 2 0x97 0x1f 251"
usleep 70000
./pc "m s 0x402 2 0x98 0x17 251"

# issue bunch reset
usleep 70000
./pc "m s 0x402 2 0x99 0x1 251"

# turn on end-of-run alert messages:
usleep 70000
./pc "m s 0x402 2 0xc 0x0 251"

usleep 70000
./showSerdes 251

exit

