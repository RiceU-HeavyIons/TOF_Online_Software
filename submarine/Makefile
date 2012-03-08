CXXFLAGS = -pipe -O2
REGFLAGS =  -O2

#all: tofNoise tofSubmarineTimings mtdNoise
all: mtdNoise

tofSubmarineTimings: sub_find_timings.cxx TsPacket.o
	$(CXX) `root-config --cflags --glibs` $(CXXFLAGS) -o $@ $^ 

tofNoise: sub_Noise.cxx TsPacket.o regMany.o
	$(CXX) `root-config --cflags --glibs` $(CXXFLAGS) -o $@ $^ 

# need to make a copy of mtdNoise.cxx and rewrite it for cosmics
#mtdNoise: mtdNoise.cxx readMTD.h TsPacket.o regMany.o
#	$(CXX) `root-config --cflags --glibs` $(CXXFLAGS) -o $@ $^ 

mtdNoise: mtdNoise.cxx readMTD.h regMany.o
	$(CXX) `root-config --cflags --glibs` $(CXXFLAGS) -o $@ $^ 


TsPacket.o: TsPacket.cpp
	$(CXX) `root-config --cflags` $(CXXFLAGS) -c $<

regMany.o: regMany.cxx regMany.h
	$(CXX) -o $@ $(REGFLAGS) -c $<

clean:
	@rm -rf Noise FindTimings mtdNoise *.o *.so

