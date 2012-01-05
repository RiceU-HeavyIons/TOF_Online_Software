#! /bin/sh

STATUS=/homes/tofp/public_html/_static/var/log/logrotate.status
CONF=/homes/tofp/UtWeb/robot/tofp.lone/logrotate.conf

/usr/sbin/logrotate -s $STATUS $CONF

