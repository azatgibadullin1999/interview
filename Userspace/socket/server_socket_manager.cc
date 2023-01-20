#include "server_socket_manager.hpp"

ServerSocketManager::ServerSocketManager(const std::string &ip) {
	pollfd	server_socket;
	int	opt_val = 1;

	if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void *>(&opt_val), sizeof(opt_val)) < 0) {
		throw std::exception();
	}
	if (ioctl(socket_, FIONBIO, reinterpret_cast<void *>(&opt_val)) < 0) {
		throw std::exception();
	}

	memset(&addr_, 0, sizeof(sockaddr_in));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(2510);
	if (!inet_aton(ip.c_str(), &addr_.sin_addr)) {
		throw std::exception();
	}
	if ((bind(socket_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_))) < 0) {
		throw std::exception();
	}
	if ((listen(socket_, 128)) < 0) {
		throw std::exception();
	}

	server_socket.fd = socket_;
	server_socket.events = POLLIN;
	pollfds_.push_back(std::move(server_socket));
}

ServerSocketManager::~ServerSocketManager() {
	for (auto it = pollfds_.begin(), ite = pollfds_.end(); it != ite; ++it) {
		close(it->fd);
	}
}

std::vector<Connection>	ServerSocketManager::poll() {
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

void		ServerSocketManager::deleteClient(Connection &con) {
	int	socket_for_delete = con.getSocket__();
	auto	it = pollfds_.begin();

	for (auto ite = pollfds_.end() ; it != ite && it->fd != socket_for_delete; ++it) { }
	close(it->fd);
	pollfds_.erase(it);
}

void		ServerSocketManager::sendAll(const std::string &msg, const Connection &from) {
	Connection	buff_of_connection;

	for (auto it = pollfds_.begin(), ite = pollfds_.end(); it != ite; ++it) {
		buff_of_connection = Connection(it->fd);
		if (it->fd != from.getSocket__()) {
			buff_of_connection.send(msg);
			if (buff_of_connection.isClosed()) {
				deleteClient(buff_of_connection);
			}
		}
	}
}

void		ServerSocketManager::addClient__() {
	pollfd	new_client;

	new_client.fd = accept(pollfds_[0].fd, NULL, NULL);
	new_client.events = POLLIN;

	pollfds_.push_back(std::move(new_client));
}
