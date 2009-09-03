
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
  export PATH=/usr/local/Trolltech/Qt-4.4.3/bin:$PATH
  export LD_LIBRARY_PATH=/usr/local/Trolltech/Qt-4.4.3/lib:$LD_LIBRARY_PATH
fi
