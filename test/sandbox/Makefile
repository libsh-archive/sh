all:

%: %.cpp
	libtool --mode=link $(CXX) -o $@ -Wall -g -O0 -I../../src $< ../../src/libsh.la -lGL

%.o: %.cpp
	$(CXX) -o $@ -Wall -g -O0 -I../../src $<


