// #include "server.hpp"

# include "../socket/server_socket_manager.hpp"


namespace {

	void	daemonizer() {
		pid_t	pid = fork();

		if (pid) {
			exit(pid > 0 ? 0 : errno);
		}
		setsid();
	}

}

namespace Server {

void	Start(const std::string &ip) {
	ServerSocketManager	serv_socket(ip);
	std::string		msg;

	daemonizer();
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