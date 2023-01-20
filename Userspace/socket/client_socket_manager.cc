#include "client_socket_manager.hpp"

ClientSocketManager::ClientSocketManager(const std::string &ip) {
	int	sock;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throw std::exception();
	}
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(12345);
	if (!inet_aton(ip.c_str(), &addr_.sin_addr)) {
		close(sock);
		throw std::exception();
	}

	if (connect(sock, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_)) < 0) {
		close(sock);
		throw std::exception();
	}
	std::cout << "start like client" << std::endl;
}

ClientSocketManager::~ClientSocketManager() {
	close(con_.getSocket__());
}

Connection	ClientSocketManager::getConnection() const {
	return con_;
}