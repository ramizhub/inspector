CC=g++
CFLAGS=-c -Wall -I./include
LDFLAGS=-lcurl
SOURCES=./src/main.cpp ./src/inspectorsrc.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=inspectorbin

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
