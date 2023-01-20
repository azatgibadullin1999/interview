#include "client_socket_manager.hpp"

ClientSocketManager::ClientSocketManager(const std::string &ip) {
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(12345);
	if (!inet_aton(ip.c_str(), &addr_.sin_addr)) {
		throw std::exception();
	}

	if (connect(socket_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_)) < 0) {
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