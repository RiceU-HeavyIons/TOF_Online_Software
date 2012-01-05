#!/bin/csh

set src=/home/data/
set dst=/data/star/tof/backup_pegasus

rsync -v --checksum --archive --update --rsh="ssh -p 4422" tof@pegasus:$src $dst

#foreach ndir (gasmon test_stand production)
#  echo $ndir
#  rsync -cavz --rsh="ssh -p 4422" pegasus:$src/$ndir $dst/$ndir
#end

