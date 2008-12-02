
if ( `hostname` == 'natsu.local' ) then
  setnev DYLD_LIBRARY_PATH `pwd`/lib
  set ANA2DIR `pwd`
  alias  ana "pushd ${ANA2DIR}; ./AnacondaII.app/Contents/MacOS/AnacondaII; popd"
endif

if ( `uname` == 'Linux' ) then
  set ANA2DIR=`pwd`
  setenv LD_LIBRARY_PATH `pwd`/lib:`pwd`/fakepcan:/homes/koheik/local/x86_64/qt/lib
endif

