CC = clang++
CFLAGS = -Wall -Wextra -Werror

EXECUTABLE_FILE = userspace

SOURCE_EXT =	.cc
OBJECT_EXT =	.o
INCLUDE_EXT =	.hpp

SOURCE_NAME =	main$(SOURCE_EXT) \
		server$(SOURCE_EXT) \
		client$(SOURCE_EXT) \
		server_socket_manager$(SOURCE_EXT) \
		client_socket_manager$(SOURCE_EXT) \
		socket_manager_base$(SOURCE_EXT) \
		connection$(SOURCE_EXT)

SOURCE_FILES =	$(foreach P, $(SOURCE_NAME), $(shell find . -name "$(P)"))
INCLUDE_FILES =	$(addprefix -I,$(SOURCE_FILES:$(SOURCE_EXT)=$(INCLUDE_EXT)))
OBJECT_FILES =	$(SOURCE_FILES:$(SOURCE_EXT)=$(OBJECT_EXT))

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
