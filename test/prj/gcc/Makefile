INCDIR=../../../inc
OUTDIR=obj
BINDIR=bin
SRCDIR=../../src

CXX=g++
CXXFLAGS=-Wall -I$(INCDIR) -DCRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
LINKER=g++
LDFLAGS=-Wall
MKDIR=mkdir

CPP_FILES := $(wildcard ../../src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

TARGET=$(BINDIR)/unittest

$(TARGET): $(OBJ_FILES)
	$(MKDIR) -p $(BINDIR)
	$(LINKER) $(LDFLAGS) -o $@ $^

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUTDIR):
	$(MKDIR) -p $(OUTDIR)
	
$(OBJ_FILES): | $(OUTDIR)

.PHONY: all
.PHONY: clean
.PHONY: debug
.PHONY: release
.PHONY: cpp11
.PHONY: cpp11_debug
.PHONY: cpp11_release

all: $(TARGET)

clean:
	rm -f $(OBJ_FILES)
	rm -f $(TARGET)

debug: CXXFLAGS+=-DDEBUG -g
debug: $(TARGET)

release: all

cpp11: CXXFLAGS+=-std=c++11
cpp11: $(TARGET)

cpp11_debug: CXXFLAGS+=-std=c++11 -DDEBUG -g
cpp11_debug: $(TARGET)

cpp11_release: cpp11
