APP_NAME = jpegtorgb565

# Directories
TARGETDIR = bin
SOURCEDIR = src
OBJECTDIR = bin/obj
INCLUDEDIR = include

STRUCT := $(shell find $(SOURCEDIR) -type d)

SOURCEDIRSTRUCT := $(filter-out %/$(INCLUDEDIR), $(STRUCT))
INCLUDEDIRSTRUCT := $(filter %/$(INCLUDEDIR), $(STRUCT))
OBJECTDIRSTRUCT := $(subst $(SOURCEDIR), $(OBJECTDIR), $(SOURCEDIRSTRUCT))

# Compillers
CC = gcc
CXX = g++

# Compiller flags
CFLAGS = $(addprefix -I,$(INCLUDEDIRSTRUCT)) -std=gnu11
CXXFLAGS = $(addprefix -I,$(INCLUDEDIRSTRUCT)) -std=gnu++11

LDLIBS = -ljpeg

# Sources & objects
SRCFILES := $(addsuffix /*, $(SOURCEDIRSTRUCT))
SRCFILES := $(wildcard $(SRCFILES))

CSOURCES := $(filter %.c, $(SRCFILES))
COBJECTS := $(subst $(SOURCEDIR), $(OBJECTDIR), $(CSOURCES:%.c=%.o))

CXXSOURCES := $(filter %.cpp, $(SRCFILES))
CXXOBJECTS := $(subst $(SOURCEDIR), $(OBJECTDIR), $(CXXSOURCES:%.cpp=%.o))

SOURCES = $(ASSOURCES) $(CSOURCES) $(CXXSOURCES)
OBJECTS = $(ASOBJECTS) $(COBJECTS) $(CXXOBJECTS)

DEPENDENCIES = $(OBJECTS:.o=.d)

# Target
TARGET = $(TARGETDIR)/$(APP_NAME)

all: make-dir $(TARGET)

$(TARGET): $(OBJECTS)
	@echo Compiling App \'$@\'...
	@$(CXX) -o $@ $(OBJECTS) $(CXXFLAGS) $(LDLIBS)

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.c
	@echo Compilling C file \'$<\' \> \'$@\'...
	@$(CC) $(CFLAGS) -c -o $@ $<

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.cpp
	@echo Compilling C++ file \'$<\' \> \'$@\'...
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

make-dir:
	@mkdir -p $(OBJECTDIRSTRUCT)

clean:
	@rm -rf $(OBJECTDIR)/*

.PHONY: clean make-dir all

-include $(DEPENDENCIES)
