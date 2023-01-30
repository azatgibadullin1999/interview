#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>

int	ft_write() {
	int	fd;
	char	buff[128];
	size_t	readed_symbols;
#ifdef NONBLOCK
	struct pollfd	pollfd;
#endif

	if ((fd = open("/dev/ft_pipe0", O_WRONLY)) == -1) {
		return 1;
	}
#ifdef NONBLOCK
	fcntl(fd, F_SETFL, O_NONBLOCK);
	pollfd.fd = fd;
	pollfd.events = POLLOUT;
#endif
	while (1) {
		if ((readed_symbols = read(STDIN_FILENO, buff, sizeof(buff))) < 0) {
			return 1;
		}
#ifdef NONBLOCK
		if (poll(&pollfd, 1, 2000) < 0) {
			return 1;
		}
		if (!(pollfd.revents & POLLOUT)) {
			continue;
		}
#endif
		if (write(fd, buff, readed_symbols) < 0) {
			return 1;
		}
	}
	close(fd);
	return 0;
}

int	ft_read() {
	int	fd;
	char	buff[128];
	size_t	readed_symbols;
#ifdef NONBLOCK
	struct pollfd	pollfd;
#endif

	if ((fd = open("/dev/ft_pipe0", O_RDONLY)) == -1) {
		return -1;
	}
#ifdef NONBLOCK
	fcntl(fd, F_SETFL, O_NONBLOCK);
	pollfd.fd = fd;
	pollfd.events = POLLIN;
#endif
	while (1) {
#ifdef NONBLOCK
		if (poll(&pollfd, 1, 2000) < 0) {
			return 1;
		}
		if (!(pollfd.revents & POLLIN)) {
			continue;
		}
#endif
		if ((readed_symbols = read(fd, buff, sizeof(buff))) < 0) {
			return 1;
		}
		if (write(STDOUT_FILENO, buff, readed_symbols) < 0) {
			return 1;
		}
	}

	close(fd);
	return 0;
}


int main(int argc, char **argv) {
	int	return_code_first, return_code_second;
	pid_t	child[2];

	for (size_t i = 0; i < 2; ++i) {
		child[i] = fork();
		if (child[i] == 0) {
			if (i == 0) {
				puts("pid writer started");
				exit(ft_write());
			} else {
				puts("pid reader started");
				exit(ft_read());
			}
		}
	}
	waitpid(child[0], &return_code_first, 0);
	waitpid(child[1], &return_code_second, 0);
	if (return_code_first || return_code_second) {
		return 1;
	}

}
