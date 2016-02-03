CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv`

SRC = $(shell find . -name *.cpp)

OBJ = $(SRC:%.cpp=%.o)

BIN = bin/cvproj

# all is the target (you would run make all from the command line). 'all' is dependent
# on $(BIN)
all: $(BIN)

#$(BIN) is dependent on objects
$(BIN): $(OBJ)
	g++ -std=c++11 -ggdb $(CFLAGS) $(OBJ) -o $(BIN) $(LDFLAGS) 

#each object file is dependent on its source file, and whenever make needs to create
# an object file, to follow this rule:
%.o: %.cpp
	g++ -std=c++11 -c -ggdb $(CFLAGS) $< -o $@ $(LDFLAGS)

#add -Wshadow to object rule to see shadowed definitions
