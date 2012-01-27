#!/bin/sh
# $Id$

echo -n "#define SVN_VERSION \"" > /tmp/version.h
svnversion -n . >> /tmp/version.h 
echo -n "\"" >> /tmp/version.h 
echo ""  >> /tmp/version.h 

# see if file is different or if it even exists
diff /tmp/version.h src/version.h
if [ $? != 0 ]; then
    /bin/mv /tmp/version.h src/version.h
else
    /bin/rm /tmp/version.h
fi

exit
