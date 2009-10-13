#!/bin/sh
# $Id$

# load the TCPU and TDIG FPGA from Eeprom 2
for ((i=21; i<40; i+=1)); do
    ./pc "m s 0x"$i"2 5 0x8a 0x69 0x96 0xa5 0x5a 250"
    usleep 70000
    for j in 40 44 48 4c 50 54 58 5c; do
	./pc "m e 0x"$j"800"$i" 5 0x8a 0x69 0x96 0xa5 0x5a 250"
	usleep 70000
    done
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./pc "m s 0x"$i"2 5 0x8a 0x69 0x96 0xa5 0x5a 250"
    usleep 70000
    for j in 40 44 48 4c 50 54 58 5c; do
	./pc "m e 0x"$j"800"$i" 5 0x8a 0x69 0x96 0xa5 0x5a 250"
	usleep 70000
    done
done

# turn off all of the Serdes channels on THUB
for ((i=91; i<99; i+=1)) do
    ./pc "m s 0x402 2 0x"$i" 0x0 250"
    usleep 70000
done

# reset all of the TDCs on each TDIG
#for ((i=21; i<40; i+=1)); do
#    for j in 40 44 48 4c 50 54 58 5c; do
#	./pc "m e 0x"$j"800"$i" 1 0x90 250"
#	./pc "m e 0x"$j"800"$i" 5 0xe 0x2 0x1 0x2 0x0 250"
#	usleep 70000
#    done
#done

#for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
#    for j in 40 44 48 4c 50 54 58 5c; do
#	./pc "m e 0x"$j"800"$i" 1 0x90 250"
#	./pc "m e 0x"$j"800"$i" 5 0xe 0x2 0x1 0x2 0x0 250"
#	usleep 70000
#    done
#done

# put TCPUs into run mode and turn on Serdes, turn off CANbus data
for ((i=21; i<40; i+=1)); do
    # set threshold on each TDIG board to 1200mV
    for ((j=10; j<18; j+=1)); do
#	./xsetThreshold 250 0x"$j" 0x"$i" 2500
	./xsetThreshold 250 0x"$j" 0x"$i" 1200
	usleep 70000
    done
    ./pc "m s 0x"$i"2 3 0xe 0x2 0xf 250"
    usleep 70000
    # set mult gate delay
    ./pc "m s 0x"$i"2 3 0xe 0x8 0xe0 250"
    usleep 70000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    # set threshold on each TDIG board to 1200mV
    for ((j=10; j<18; j+=1)); do
#	./xsetThreshold 250 0x"$j" 0x"$i" 2500
	./xsetThreshold 250 0x"$j" 0x"$i" 1200
	usleep 70000
    done
    ./pc "m s 0x"$i"2 3 0xe 0x2 0xf 250"
    usleep 70000
    # set mult gate delay
    ./pc "m s 0x"$i"2 3 0xe 0x8 0xe0 250"
    usleep 70000
done

# turn on THUB serdes channels and put into regular trigger mode
./pc "m s 0x402 2 0x91 0x1f 250"
usleep 70000
./pc "m s 0x402 2 0x92 0x1f 250"
usleep 70000
./pc "m s 0x402 2 0x93 0x1f 250"
usleep 70000
./pc "m s 0x402 2 0x94 0x1f 250"
usleep 70000
./pc "m s 0x402 2 0x95 0x1f 250"
usleep 70000
./pc "m s 0x402 2 0x96 0x1f 250"
usleep 70000
./pc "m s 0x402 2 0x97 0x1f 250"
usleep 70000
./pc "m s 0x402 2 0x98 0x13 250"

# check that all Serdes channels sync'd
for ((i=91; i<99; i+=1)) do
    usleep 70000
    ./pc "m s 0x404 1 0x"$i" 250"
done

# issue bunch reset
usleep 70000
./pc "m s 0x402 2 0x99 0x1 251"


exit

