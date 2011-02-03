
if [ `hostname` = 'natsu.local' ]; then
  export DYLD_LIBRARY_PATH=`pwd`/lib:`pwd`/fakepcan
  ANA2DIR=`pwd`
  alias  ana="pushd ${ANA2DIR}; ./AnacondaII.app/Contents/MacOS/AnacondaII; popd"
fi
if [ `uname` = 'Darwin' ]; then
  export DYLD_LIBRARY_PATH=`pwd`/lib:`pwd`/fakepcan
  ANA2DIR=`pwd`
  alias  ana="pushd ${ANA2DIR}; ./AnacondaII.app/Contents/MacOS/AnacondaII; popd"
fi

if [ `hostname` = 'pisces.rhip.utexas.edu' ]; then
  export LD_LIBRARY_PATH=`pwd`/lib:`pwd`/fakepcan:/homes/koheik/local/x86_64/qt/lib
fi

if [ `hostname` = 'scorpio.rhip.utexas.edu' ]; then
  export PATH=/home/tof/c/qt/bin:$PATH
  export LD_LIBRARY_PATH=`pwd`/lib:`pwd`/fakepcan:/home/tof/c/qt/lib
fi

if [ `hostname` = 'cancer.rhip.utexas.edu' ]; then
  export PATH=/usr/local/Trolltech/Qt-4.7.1/bin:$PATH
  export LD_LIBRARY_PATH=/usr/local/Trolltech/Qt-4.7.1/lib:$LD_LIBRARY_PATH
fi

if [ `hostname` = 'tofcontrol.starp.bnl.gov' ]; then
  export PATH=/usr/local/Trolltech/Qt-4.7.1/bin:$PATH
  export LD_LIBRARY_PATH=/usr/local/Trolltech/Qt-4.7.1/lib:$LD_LIBRARY_PATH
fi
