#!/bin/sh

# NW:
for ((i=21; i<40; i+=1)); do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 251"
    usleep 70000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 251"
    usleep 70000
done

# SW:
for ((i=21; i<40; i+=1)); do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 255"
    usleep 70000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 255"
    usleep 70000
done

# NE:
for ((i=21; i<40; i+=1)); do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 254"
    usleep 70000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 254"
    usleep 70000
done

# SE:
for ((i=21; i<40; i+=1)); do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 250"
    usleep 70000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./pc "m s 0x"$i"2 3 0xe 0x9 0x0 250"
    usleep 70000
done


exit

