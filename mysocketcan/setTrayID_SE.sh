#!/bin/sh
# $Id: setTrayID_SE.sh 894 2014-02-06 17:53:26Z jschamba $

./cansend can6 402#2600006200
usleep 200000
./cansend can6 402#2601006300
usleep 200000
./cansend can6 402#2602006000
usleep 200000
./cansend can6 402#2603006100
usleep 200000

./cansend can6 402#2604006900
usleep 200000
./cansend can6 402#2605006700
usleep 200000
./cansend can6 402#2606006400
usleep 200000
./cansend can6 402#2607006500
usleep 200000

./cansend can6 402#2608006800
usleep 200000
./cansend can6 402#2609006b00
usleep 200000
./cansend can6 402#260a006a00
usleep 200000
./cansend can6 402#260b006600
usleep 200000

./cansend can6 402#260c006e00
usleep 200000
./cansend can6 402#260d006f00
usleep 200000
./cansend can6 402#260e006c00
usleep 200000
./cansend can6 402#260f006d00
usleep 200000

./cansend can6 402#2610007200
usleep 200000
./cansend can6 402#2611007300
usleep 200000
./cansend can6 402#2612007000
usleep 200000
./cansend can6 402#2613007100
usleep 200000

./cansend can6 402#2614007600
usleep 200000
./cansend can6 402#2615007700
usleep 200000
./cansend can6 402#2616007400
usleep 200000
./cansend can6 402#2617007500
usleep 200000

./cansend can6 402#2618003e00
usleep 200000
./cansend can6 402#2619003f00
usleep 200000
./cansend can6 402#261a007800
usleep 200000
./cansend can6 402#261b003d00
usleep 200000

./cansend can6 402#261c007f00
usleep 200000
./cansend can6 402#261d007f00
usleep 200000
./cansend can6 402#261e004000
usleep 200000
./cansend can6 402#261f004100
usleep 200000

# enable or disable channels
./cansend can6 402#2620001f00 #A
usleep 200000
./cansend can6 402#2621001f00 #B
usleep 200000
./cansend can6 402#2622001f00 #C
usleep 200000
./cansend can6 402#2623001f00 #D
usleep 200000

./cansend can6 402#2624001f00 #E
usleep 200000
./cansend can6 402#2625001f00 #F
usleep 200000
./cansend can6 402#2626001f00 #G
usleep 200000
./cansend can6 402#2627001300 #H
usleep 200000

exit
