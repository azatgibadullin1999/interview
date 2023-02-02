// #include "server.hpp"

# include <fcntl.h>
# include "../socket/server_socket_manager.hpp"


namespace {

	void	daemonizer() {
		int	fd;
		pid_t	pid = fork();

		if (pid) {
			exit(pid > 0 ? 0 : errno);
		}
		fd = open("./server_logs.txt", O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		setsid();
	}

}

namespace Server {

void	Start(const std::string &ip) {
	ServerSocketManager	serv_socket(ip);
	std::string		msg;

	daemonizer();
	std::cout << "server started" << std::endl;
	while (true) {
		std::vector<Connection>	connections = serv_socket.poll();
		if (!connections.size()) {
			continue;
		}
		for (auto it = connections.begin(), ite = connections.end(); it != ite; ++it) {
			msg = it->recive();
			if (it->isClosed()) {
				serv_socket.deleteClient(*it);
				continue;
			}

			serv_socket.sendAll(msg, *it);
		}
	}
}

}