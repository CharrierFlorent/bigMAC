CC=gcc
RM=rm -f

CFLAGS=-W -Wall -std=gnu99 -g

SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
BASENAMES=$(basename $(SOURCES)) 
OBJECTS=$(addsuffix .o , $(BASENAMES))
EXEC=bigmac

all: $(EXEC)

.c.o:
	@echo "\033[1;33m=== COMPILING $*.c INTO $@ ===\033[0m"
	$(CC) $(CFLAGS) -c $*.c
	@echo

$(EXEC): $(OBJECTS)
	@echo "\033[1;32m=== LINKING OBJECTS INTO $(EXEC) ===\033[0m"
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

clean:
	@echo "\033[1;31m=== CLEANING OBJECTS ===\033[0m"
	$(RM) $(OBJECTS) $(EXEC)

display:
	@echo "SOURCES:" $(SOURCES)
	@echo "HEADERS:" $(HEADERS)
	@echo "BASENAMES:" $(BASENAMES)
	@echo "OBJECTS:" $(OBJECTS)
	@echo "EXEC:" $(EXEC)

depend: $(SOURCES) $(HEADERS)
	@echo "\033[1;34m=== UPDATING DEPENDENCIES ===\033[0m"
	gcc $(CFLAGS) -MM $(SOURCES) | tee depend

valgrind: 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./$(EXEC)

-include depend
