#!/bin/sh

for ((i=21; i<40; i+=1)); do
    ./xp_config_pm_l 0 0x10 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x11 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x12 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x13 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x14 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x15 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x16 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x17 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 1 0x10 0x"$i" hptdc/cfig20091029a.txt 254
    usleep 300000
    ./xp_config_pm_l 1 0x14 0x"$i" hptdc/cfig20091029a.txt 254
    usleep 300000
done


# 3e (tray 95) turned off
for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d; do
    ./xp_config_pm_l 0 0x10 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x11 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x12 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x13 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x14 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x15 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x16 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 0 0x17 0x"$i" hptdc/cfig20091029.txt 254
    usleep 300000
    ./xp_config_pm_l 1 0x10 0x"$i" hptdc/cfig20091029a.txt 254
    usleep 300000
    ./xp_config_pm_l 1 0x14 0x"$i" hptdc/cfig20091029a.txt 254
    usleep 300000
done

exit

    
