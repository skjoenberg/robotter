# Compile and link flags
CXX      =  g++ -g
CXXFLAGS = `pkg-config opencv --cflags playerc++` -Wall
LIBS     = `pkg-config opencv --libs playerc++`

# Name of executable
EXECUTABLE = afstand

# Files
SOURCES = nyafstand.cc movement.cc
OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))

# Linking
all: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

# Compilation
%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -f $(OBJECTS) *~ $(EXECUTABLE)
