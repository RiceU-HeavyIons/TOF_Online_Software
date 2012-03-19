#!/bin/sh
# $Id$

# NW
for ((i=21; i<40; i+=1)); do
    ./xp_config_scripted_l 0x"$i" trayscript.txt 251
    usleep 300000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./xp_config_scripted_l 0x"$i" trayscript.txt 251
    usleep 300000
done

./pc "m e 0x1fc8007f 5 0xe 0x2 0x1 0x2 0x0 251"
sleep 3

# SW
for ((i=21; i<40; i+=1)); do
    ./xp_config_scripted_l 0x"$i" trayscript.txt 255
    usleep 300000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./xp_config_scripted_l 0x"$i" trayscript.txt 255
    usleep 300000
done

./pc "m e 0x1fc8007f 5 0xe 0x2 0x1 0x2 0x0 255"
sleep 3

# NE
for ((i=21; i<40; i+=1)); do
    ./xp_config_scripted_l 0x"$i" trayscript.txt 254
    usleep 300000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d; do # Tray 95 disabled
    ./xp_config_scripted_l 0x"$i" trayscript.txt 254
    usleep 300000
done

./pc "m e 0x1fc8007f 5 0xe 0x2 0x1 0x2 0x0 254"
sleep 3

# SE
for ((i=21; i<27; i+=1)); do # Tray 102 disabled
    ./xp_config_scripted_l 0x"$i" trayscript.txt 250
    usleep 300000
done

for ((i=28; i<40; i+=1)); do
    ./xp_config_scripted_l 0x"$i" trayscript.txt 250
    usleep 300000
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./xp_config_scripted_l 0x"$i" trayscript.txt 250
    usleep 300000
done

./pc "m e 0x1fc8007f 5 0xe 0x2 0x1 0x2 0x0 250"
sleep 3


# VPD West:
./xp_config_scripted_l 0x20 vpdscript.txt 252
sleep 1
./pc "m e 0x1fc80020 5 0xe 0x2 0x1 0x2 0x0 252"
sleep 2

# VPD East:
./xp_config_scripted_l 0x20 vpdscript.txt 253
sleep 1
./pc "m e 0x1fc80020 5 0xe 0x2 0x1 0x2 0x0 253"
sleep 2



exit

    
