#include "socket_manager.hpp"

SocketManager::SocketManager() {
	pollfd	server_socket;
	int	listen_fd;
	int	opt_val = 1;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throw std::exception();
	}
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void *>(&opt_val), sizeof(opt_val)) < 0) {
		throw std::exception();
	}
	if (ioctl(listen_fd, FIONBIO, reinterpret_cast<void *>(&opt_val)) < 0) {
		throw std::exception();
	}

	memset(&addr_, 0, sizeof(sockaddr_in));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(1488);
	inet_aton("127.0.0.1", &addr_.sin_addr);

	if ((bind(listen_fd, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_))) < 0) {
		throw std::exception();
	}
	if ((listen(listen_fd, 128)) < 0) {
		throw std::exception();
	}

	server_socket.fd = listen_fd;
	server_socket.events = POLLIN;
	pollfds_.push_back(std::move(server_socket));
}

SocketManager::~SocketManager() { }

std::vector<Connection>	SocketManager::poll() {
	int num_of_events = ::poll(pollfds_.data(), pollfds_.size(), 2000);
	std::vector<Connection>	ret_connections;

	ret_connections.reserve(num_of_events);

	if (num_of_events == -1) {
		throw std::exception();
	}
	if (num_of_events == 0) {
		return std::vector<Connection>();
	}
	if ((pollfds_[0].revents & POLLIN) != 0) {
		num_of_events -= 1;
		addClient__();
	}
	for (auto it = ++pollfds_.begin(), ite = pollfds_.end(); it != ite; ++it) {
		if (it->revents == POLLIN) {
			ret_connections.push_back(Connection(it->fd));
		}
	}
	return ret_connections;
}

void		SocketManager::deleteClient(Connection &con) {
	int	socket_for_delete = con.getSocket__();
	auto	it = pollfds_.begin();

	for (auto ite = pollfds_.end() ; it != ite && it->fd != socket_for_delete; ++it) { }
	close(it->fd);
	pollfds_.erase(it);
}

void		SocketManager::sendAll(const std::string &msg, const Connection &from) {
	for (auto it = pollfds_.begin(), ite = pollfds_.end(); it != ite; ++it) {
		if (it->fd != from.getSocket__()) {
			Connection(it->fd).send(msg);
		}
	}
}

void		SocketManager::addClient__() {
	pollfd	new_client;

	new_client.fd = accept(pollfds_[0].fd, NULL, NULL);
	new_client.events = POLLIN;

	pollfds_.push_back(std::move(new_client));
}
