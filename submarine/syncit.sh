#!/bin/sh

rsync -rlCzv --exclude="*.dat" /home/tof/mtdData/ tofp@lone:public_html/_static/mtdData
