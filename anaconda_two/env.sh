
if [ `hostname` = 'natsu.local' ]; then
  export DYLD_LIBRARY_PATH=`pwd`/lib
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

