#!/bin/sh
# $Id$

# NorthWest: GEM at 0x23
for ((i=21; i<40; i+=1)); do
    if [ "$i" = "23" ]; then
	./x_config_pm 0 0x10 0x"$i" hptdc/cfig20160225.txt 1;
	sleep 1;
	for ((j=12; j<17; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 1;
	    sleep 1;
	done
    else
	for ((j=10; j<18; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 1;
	    sleep 1;
	done
    fi
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 1; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 1; 
    sleep 1; 
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do 
    for ((j=10; j<18; j+=1)); do
	./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 1;
	sleep 1;
    done
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 1; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 1; 
    sleep 1; 
done

# NorthEast: GEM at 0x3c
for ((i=21; i<40; i+=1)); do
    for ((j=10; j<18; j+=1)); do
	./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 2;
	sleep 1;
    done
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 2; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 2; 
    sleep 1; 
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do 
    if [ "$i" = "3c" ]; then
	./x_config_pm 0 0x10 0x"$i" hptdc/cfig20160225.txt 2;
	sleep 1;
	for ((j=12; j<17; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 2;
	    sleep 1;
	done
    else
	for ((j=10; j<18; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 2;
	    sleep 1;
	done
    fi
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 2; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 2; 
    sleep 1; 
done

# SouthWest: GEM at 0x32
for ((i=21; i<40; i+=1)); do
    if [ "$i" = "32" ]; then
	./x_config_pm 0 0x10 0x"$i" hptdc/cfig20160225.txt 3;
	sleep 1;
	for ((j=12; j<17; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 3;
	    sleep 1;
	done
    else
	for ((j=10; j<18; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 3;
	    sleep 1;
	done
    fi
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 3; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 3; 
    sleep 1; 
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do 
    for ((j=10; j<18; j+=1)); do
	./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 3;
	sleep 1;
    done
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 3; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 3; 
    sleep 1; 
done

# SouthEast: GEM at 0x2d
for ((i=21; i<40; i+=1)); do
    for ((j=10; j<18; j+=1)); do
	./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 6;
	sleep 1;
    done
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 6; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 6; 
    sleep 1; 
done

for i in 2a 2b 2c 2d 2e 2f 3a 3b 3c 3d 3e; do 
    if [ "$i" = "2d" ]; then
	./x_config_pm 0 0x10 0x"$i" hptdc/cfig20160225.txt 6;
	sleep 1;
	for ((j=12; j<17; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 6;
	    sleep 1;
	done
    else
	for ((j=10; j<18; j+=1)); do
	    ./x_config_pm 0 0x"$j" 0x"$i" hptdc/cfig20160225.txt 6;
	    sleep 1;
	done
    fi
    ./x_config_pm 1 0x10 0x"$i" hptdc/cfig20160225a.txt 6; 
    sleep 1; 
    ./x_config_pm 1 0x14 0x"$i" hptdc/cfig20160225a.txt 6; 
    sleep 1; 
done

exit
