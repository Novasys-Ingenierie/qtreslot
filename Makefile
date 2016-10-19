PLUGIN = qtreslot

HEADERS = \
	clang_headers.h \
	AddActionCall.h \
	ConnectCall.h \
	Context.h \
	DisconnectCall.h \
	SingleShotCall.h \
	SlotCall.h

SOURCES = \
	main.cpp \
	AddActionCall.cpp \
	ConnectCall.cpp \
	Context.cpp \
	DisconnectCall.cpp \
	SingleShotCall.cpp \
	SlotCall.cpp

OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LLVM_COMPONENTS = all
LLVM_CXXFLAGS = $(shell llvm-config --cxxflags)
LLVM_LDFLAGS = $(shell llvm-config --ldflags)

CXXFLAGS = -std=c++11 -fPIC -Wall
CXXFLAGS += -fvisibility-inlines-hidden -fno-exceptions -fno-rtti
CXXFLAGS += $(LLVM_CXXFLAGS)

LDFLAGS = -shared -Wl,-undefined,dynamic_lookup $(LLVM_LDFLAGS)

all: $(PLUGIN).so

$(PLUGIN).so: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(PLUGIN).so $(OBJECTS)

.PHONY: all clean
