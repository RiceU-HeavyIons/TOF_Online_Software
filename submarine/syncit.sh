#!/bin/sh

rsync -azv --exclude="*.dat" /home/data/mtdData/ tofp@lone:public_html/_static/mtdData
