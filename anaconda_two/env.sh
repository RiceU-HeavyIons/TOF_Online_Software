
if [ `hostname` = 'natsu.local' ]; then
  export DYLD_LIBRARY_PATH=`pwd`/lib
  ANA2DIR=`pwd`
  alias  ana="pushd ${ANA2DIR}; ./AnacondaII.app/Contents/MacOS/AnacondaII; popd"
fi

if [ `uname` = 'Linux' ]; then
  export LD_LIBRARY_PATH=`pwd`/lib:`pwd`/fakepcan:/homes/koheik/local/x86_64/qt/lib
fi

