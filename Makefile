PROGRAM     = Simetra
CXX         = g++

SRCDIR      = src
BUILDDIR    = build

SRCS        := $(shell find $(SRCDIR) -name *.cpp)
OBJS        := $(SRCS:%=$(BUILDDIR)/%.o)
DEPS        := $(OBJS:.o=.d)

CXXFLAGS    = -g -Wall -Wextra -pthread -std=c++11 -O3
LDLIBS      = -framework OpenGL -lGLEW -lglfw3 -lm -framework Cocoa \
              -framework IOKit -framework CoreFoundation -framework CoreVideo


$(BUILDDIR)/$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDLIBS)

$(BUILDDIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)


-include $(DEPS)

MKDIR_P ?= mkdir -p
