#!/bin/sh
# $Id: hptdc_MTD.sh 891 2014-01-15 15:48:11Z jschamba $

# 5 tray backlegs:
for ((i=21; i<28; i+=1)); do
    ./x_config_pm 0 0x10 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x11 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x12 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x14 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x15 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20091029a.txt 107;
    sleep 1;
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20091029a.txt 107;
    sleep 1;
done

for ((i=35; i<37; i+=1)); do
    ./x_config_pm 0 0x10 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x11 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x12 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x14 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x15 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20091029a.txt 104;
    sleep 1;
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20091029a.txt 104;
    sleep 1;
done

for ((i=38; i<40; i+=1)); do
    ./x_config_pm 0 0x10 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x11 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x12 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x14 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x15 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20091029a.txt 104;
    sleep 1;
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20091029a.txt 104;
    sleep 1;
done

for i in 2a 2b; do
    ./x_config_pm 0 0x10 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x11 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x12 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x14 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x15 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20091029a.txt 107;
    sleep 1;
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20091029a.txt 107;
    sleep 1;
done

for i in 3a 3b 3c 3d 3e; do
    ./x_config_pm 0 0x10 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x11 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x12 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x14 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x15 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20091029a.txt 104;
    sleep 1;
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20091029a.txt 104; 
    sleep 1;
done

# 3-tray backlegs
for i in 2c 2d 2e 2f; do
    ./x_config_pm 0 0x10 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x11 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 0 0x14 0x"$i" hptdc/cfig20091029.txt 107;
    sleep 1;
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20091029a.txt 107;
    sleep 1;
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20091029a.txt 107;
    sleep 1;
done

for ((i=30; i<35; i+=1)); do
    ./x_config_pm 0 0x10 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x11 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 0 0x14 0x"$i" hptdc/cfig20091029.txt 104;
    sleep 1;
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20091029a.txt 104;
    sleep 1;
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20091029a.txt 104;
    sleep 1;
done

exit
