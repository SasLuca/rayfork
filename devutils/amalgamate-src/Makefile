PROGRAM = amalgamate

SRCS := Amalgamate.cpp juce_core_amalgam.cpp

CXXLD ?= $(CXX)
CXXFLAGS ?= -O2
CXXFLAGS += -std=c++11 -pthread -fno-strict-aliasing
CXXFLAGS += -Wall -Wextra -Wno-ignored-qualifiers
LIBS += -ldl -lrt
LDFLAGS += -pthread


all: $(PROGRAM)

$(PROGRAM): $(SRCS:.cpp=.o)
	$(CXXLD) -o $@ $(LDFLAGS) $^ $(LIBS)

%.o: %.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $<

clean:
	$(RM) -v $(PROGRAM) *.gch *.o

check: $(PROGRAM)
	@# just check if it's able to at least output the usage text
	./$(PROGRAM) -h


.PHONY: all clean check
