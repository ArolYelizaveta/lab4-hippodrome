CXX = g++
CXXFLAGS = -std=c++14 -Wall -Iincludes
LDFLAGS = -lsqlite3
TEST_LDFLAGS = -lsqlite3 -lgtest -lgtest_main -pthread

SRCDIR = src
INCDIR = includes
BINDIR = bin
BUILDDIR = build
TESTDIR = tests
TEST_BUILDDIR = test_build

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
TARGET = $(BINDIR)/hippodrome

TEST_SOURCES = $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
TEST_OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(TEST_BUILDDIR)/%.o,$(TEST_SOURCES))
TEST_TARGET = $(TEST_BUILDDIR)/run_tests

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJECTS) $(TEST_BUILDDIR)/test_main.o $(TEST_BUILDDIR)/test_database.o $(TEST_BUILDDIR)/test_businesslogic.o
	@mkdir -p $(TEST_BUILDDIR)
	$(CXX) $^ -o $(TEST_TARGET) $(TEST_LDFLAGS)

$(TEST_BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(TEST_BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BUILDDIR)/test_main.o: $(TESTDIR)/test_main.cpp
	@mkdir -p $(TEST_BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BUILDDIR)/test_database.o: $(TESTDIR)/test_database.cpp
	@mkdir -p $(TEST_BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BUILDDIR)/test_businesslogic.o: $(TESTDIR)/test_businesslogic.cpp
	@mkdir -p $(TEST_BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILDDIR) $(BINDIR) $(TEST_BUILDDIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run