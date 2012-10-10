  (c) SYSTEC electronic GmbH, D-07973 Greiz, August-Bebel-Str. 29
        www.systec-electronic.com

                                                       2012-07-06

        SocketCAN Driver for USB-CANmodul series
        =========================================

Requirements
-------------

* Linux Kernel version >= 2.6.32

* Following kernel options have to be set:

CONFIG_CAN=m
CONFIG_CAN_RAW=m
CONFIG_CAN_BCM=m
CONFIG_CAN_DEV=m
CONFIG_CAN_CALC_BITTIMING=y

* CAN utilities from the SocketCAN repository for first tests

$ git clone git://gitorious.org/linux-can/can-utils.git
$ cd can-utils
$ make

  Old repository on berlios:
	$ svn checkout svn://svn.berlios.de/socketcan/trunk socketcan
	$ cd socketcan/can-utils
	$ make


Supported features
-------------------

* CAN frame reception and transmission:
  + standard frame format (11 bit identifier)
  + extended frame format (29 bit identifier)
  + remote transmit request frames (RTR)

* Supported CAN controller states:
  + CAN_STATE_ERROR_ACTIVE  (CAN bus runs error-free)
  + CAN_STATE_ERROR_WARNING (error counters reached warning limit)
  + CAN_STATE_ERROR_PASSIVE (node sends passive error frames)
  + CAN_STATE_BUS_OFF       (node does not send any frames,
                             i.e. it is virtually disconnected from bus)

* Supported SocketCAN error frame flags
  (not to mix up with CAN error frames according to CAN specification)
    struct can_frame frame;
    ....
    if (frame.can_id & CAN_ERR_FLAG) {
      if (frame.can_id & CAN_ERR_BUSOFF) {
        /* BUS OFF */ }
      if (frame.can_id & CAN_ERR_RESTARTED) {
        /* recover from BUS OFF */ }
      if (frame.can_id & CAN_ERR_CRTL) {
        if (frame.data[1] & (CAN_ERR_CRTL_RX_WARNING |
            CAN_ERR_CRTL_TX_WARNING)) {
          /* warning limit reached */ }
        if (frame.data[1] & (CAN_ERR_CRTL_RX_PASSIVE |
            CAN_ERR_CRTL_TX_PASSIVE)) {
          /* error passive state entered */ }
        if (frame.can_id & CAN_ERR_PROT) {
          if (frame.data[2] & CAN_ERR_PROT_ACTIVE) {
            /* error active state entered */ }
        }
      }
    }

* Supported CAN controller modes
  + CAN_CTRLMODE_3_SAMPLES  (Triple sampling mode)

* Tx echo is implemented in driver.


Limitations
------------

* Firmware version >=4.06 must be installed on USB-CANmodul.
  In case of an older firmware version, please connect the USB-CANmodul to a
  Windows PC with a recent driver version.
* There is currently no way to read out or set the digital I/Os of the
  user port or the CAN port (signals EN, /STB, /ERR, /TRM).
* No support for the obsolete modules GW-002 and GW-001.
  This is not planned at all.
* Planned CAN controller modes
  + CAN_CTRLMODE_LOOPBACK   (Loopback mode)
  + CAN_CTRLMODE_LISTENONLY (Listen-only mode)


Build the driver
-----------------

Run make within the source directory

$ cd systec_can
$ make


Load the driver from the local source directory
------------------------------------------------

1. Load basic CAN drivers

$ sudo modprobe can_raw
$ sudo modprobe can_dev

2. Install firmware

$ sudo make firmware_install

3. Load USB-CANmodul driver

$ sudo insmod systec_can.ko

- OR -

Install the driver and firmware system-wide
--------------------------------------------

$ sudo make modules_install
$ sudo make firmware_install

The kernel module should now be loaded automatically by udev
when the device is connected.


Run basic tests
----------------

1. Connect the USB-CANmodul to the PC

2. Set bitrate to 125kBit/s

$ ip link set can0 type can bitrate 125000
- OR if CONFIG_CAN_CALC_BITTIMING is undefined -
$ ip link set can0 type can tq 500 prop-seg 6 phase-seg1 7 phase-seg2 2

3. Start up the CAN interface

$ ifconfig can0 up

4. Dump the traffic on the CAN bus

$ cd can-utils
$ ./candump can0

to display error frames (option -e is supported in newer candump versions only):

$ ./candump -e can0,0:0,#FFFFFFFF

5. Transmit one CAN frame

$ cd can-utils
$ ./cangen -n 1 -I 640 -L 8 -D 4000100000000000 can0

6. Print out some statistics

$ ip -details -statistics link show can0

7. Restart CAN channel in case of bus-off (i.e. short circuit)

$ ip link set can0 type can restart


Hardware address
-----------------

The hardware address (like the MAC address of Ethernet controllers)
of each CAN channel as shown with
`ip link show can0` or `ifconfig can0` is formed the following way:

S0:S1:S2:S3:DN:CN

Sx - Serial Number in Hex with S0 contains the most significant byte
DN - Device Number
CN - Channel Number (00 - CAN1, 01 - CAN2)

The unique hardware address can be used by a special udev rule to
assign stable interface names and numbers.

Example for udev rule (file /etc/udev/rules.d/20-systec-can.rules):

# USB device 0x0878:0x1101 (systec_can)
# device number 01, first CAN channel -> can10
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="systec_can", \
ATTR{address}=="??:??:??:??:01:00", KERNEL=="can*", NAME="can10"

# USB device 0x0878:0x1101 (systec_can)
# device number 01, second CAN channel -> can11
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="systec_can", \
ATTR{address}=="??:??:??:??:01:01", KERNEL=="can*", NAME="can11"


Further information
--------------------

[1] CAN utilities
    http://gitorious.org/linux-can/can-utils

[2] Linux Kernel Source Code Documentation/networking/can.txt
    http://git.kernel.org/?p=linux/kernel/git/torvalds/linux.git;a=blob;f=Documentation/networking/can.txt;hb=HEAD

[3] Talk about SocketCAN - CAN Driver Interface under Linux
    (German, but slides in English)
    http://chemnitzer.linux-tage.de/2012/vortraege/1044

[4] libsocketcan V0.0.8
    Helper library for CAN interface configuration (e.g. bitrate)
    over netlink API.
    http://www.pengutronix.de/software/libsocketcan/download/

