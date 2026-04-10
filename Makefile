CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iincludes
LDFLAGS = -lsqlite3

SRCDIR = src
INCDIR = includes
BINDIR = bin
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
TARGET = $(BINDIR)/hippodrome

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(BINDIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run