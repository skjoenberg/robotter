# Compile and link flags
CXX      =  g++
CXXFLAGS = `pkg-config opencv --cflags` -Wall
LIBS     = `pkg-config opencv --libs`

# Name of executable
EXECUTABLE = farve

# Files
SOURCES = farve.cc
OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))

# Linking
all: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

# Compilation
%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -f $(OBJECTS) *~ $(EXECUTABLE)
