// #include "server.hpp"

# include "../socket/server_socket_manager.hpp"

namespace Server {

void	Start(const std::string &ip) {
	ServerSocketManager	serv_socket(ip);
	std::string		msg;

	std::cout << "started like server" << std::endl;
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