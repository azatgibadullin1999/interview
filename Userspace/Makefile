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

INCLUDE_NAME =	server$(INCLUDE_EXT) \
		client$(INCLUDE_EXT) \
		server_socket_manager$(INCLUDE_EXT) \
		client_socket_manager$(INCLUDE_EXT) \
		socket_manager_base$(INCLUDE_EXT) \
		connection$(INCLUDE_EXT)

SOURCE_FILES =	$(foreach P, $(SOURCE_NAME), $(shell find . -name "$(P)"))
INCLUDE_FILES =	$(addprefix -I,$(foreach P, $(INCLUDE_NAME), $(shell find . -name "$(P)")))
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
