#!/bin/csh

set src=/home/data/
set dst=/data/star/tof/backup_scorpio

rsync -v --checksum --archive --update --rsh="ssh -p 4422" tof@scorpio:$src $dst

