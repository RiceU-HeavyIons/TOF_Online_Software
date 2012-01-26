#!/bin/sh

for ((i=21; i<27; i+=1)); do
    ./xp_config_pm_l 0 0x10 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x11 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x12 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x13 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x14 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x15 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x16 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x17 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 1 0x10 0x"$i" hptdc/cfig20091029a.txt 250
    sleep 1
    ./xp_config_pm_l 1 0x14 0x"$i" hptdc/cfig20091029a.txt 250
    sleep 1
done

# 0x27 (tray 102) turned off
for ((i=28; i<40; i+=1)); do
    ./xp_config_pm_l 0 0x10 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x11 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x12 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x13 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x14 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x15 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x16 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x17 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 1 0x10 0x"$i" hptdc/cfig20091029a.txt 250
    sleep 1
    ./xp_config_pm_l 1 0x14 0x"$i" hptdc/cfig20091029a.txt 250
    sleep 1
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do
    ./xp_config_pm_l 0 0x10 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x11 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x12 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x13 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x14 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x15 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x16 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 0 0x17 0x"$i" hptdc/cfig20091029.txt 250
    sleep 1
    ./xp_config_pm_l 1 0x10 0x"$i" hptdc/cfig20091029a.txt 250
    sleep 1
    ./xp_config_pm_l 1 0x14 0x"$i" hptdc/cfig20091029a.txt 250
    sleep 1
done

exit

    
