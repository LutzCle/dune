CXX		= g++
CXXFLAGS  = -std=c++11 -g -Wall -O2 -MD -I ../../
LDFLAGS = -L ../../libdune -ldune -lpthread -ltcmalloc

all: hash

hash: hash.o ../../libdune/libdune.a
	$(CXX) $(CXXFLAGS) $(<) -o $(@) $(LDFLAGS)

clean:
	rm -f *.o *.d hash

-include *.d
