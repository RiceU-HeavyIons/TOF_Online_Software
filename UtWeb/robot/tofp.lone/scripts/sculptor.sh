#! /bin/csh

set src=/home/data/
set dst=/data2/star/tof/backup_sculptor

rsync -v --checksum --archive --update --rsh="ssh -p 4422" tof@sculptor:$src $dst

#foreach ndir (gasmon test_stand production)
#  echo $ndir
#  rsync -cavz --rsh="ssh -p 4422" sculptor:$src/$ndir $dst/$ndir
#end

