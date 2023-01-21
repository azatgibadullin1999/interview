#include "client_socket_manager.hpp"

ClientSocketManager::ClientSocketManager(const std::string &ip) {
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(2510);
	if (!inet_aton(ip.c_str(), &addr_.sin_addr)) {
		throw std::exception();
	}
	if (connect(socket_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_)) < 0) {
		throw ft::ConnectionError();
	}
	memset(&pollfds_, 0, sizeof(pollfds_));
	pollfds_[0].fd = STDIN_FILENO;
	pollfds_[0].events = POLLIN;
	pollfds_[1].fd = socket_;
	pollfds_[1].events = POLLIN;
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

ClientSocketManager::~ClientSocketManager() { }

Connection	ClientSocketManager::getConnection() const {
	return Connection(socket_);
}

int		ClientSocketManager::poll() {
	int num_of_events = ::poll(pollfds_, 2, 2000);

	if (num_of_events == -1) {
		throw std::exception();
	}
	if (num_of_events == 0) {
		return 0;
	}
	if ((pollfds_[1].revents & POLLIN) != 0) {
		return 1;
	}
	if ((pollfds_[0].revents & POLLIN) != 0) {
		return 2;
	}
	return -1;
}

void		ClientSocketManager::endSession() {
	std::cout << "connection is lost" << std::endl;
	exit(1);
}