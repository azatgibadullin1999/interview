CC = gcc
CFLAGS = -O0

EXECUTABLE_FILE = decode

SOURCE_EXT =	.c
OBJECT_EXT =	.o
INCLUDE_EXT =

SOURCE_NAME =	decode$(SOURCE_EXT)
		
SOURCE_FILES =	$(foreach P, $(SOURCE_NAME), $(shell find . -name "$(P)"))
OBJECT_FILES =	$(SOURCE_FILES:$(SOURCE_EXT)=$(OBJECT_EXT))
# INCLUDE_FILES =	$(addprefix -I,$(SOURCE_FILES:$(SOURCE_EXT)=$(INCLUDE_EXT)))

all: $(EXECUTABLE_FILE)

$(EXECUTABLE_FILE): $(OBJECT_FILES)
	$(CC) -o $(EXECUTABLE_FILE) $^

%$(OBJECT_EXT): %$(SOURCE_EXT)
	$(CC) $(CFLAGS) $(INCLUDE_FILES) -c -o $@ $<

clean:
	rm -rf $(OBJECT_FILES)

fclean: clean
	rm -rf $(EXECUTABLE_FILE)

re: fclean all