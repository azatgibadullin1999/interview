#include "connection.hpp"

Connection::Connection(int sock) :
	socket_(sock), is_closed_(false) { }

Connection::Connection(const Connection &other) :
	socket_(other.socket_), is_closed_(other.is_closed_) { }

Connection	&Connection::operator=(const Connection &other) {
	socket_ = other.socket_;
	is_closed_ = other.is_closed_;
	return *this;
}

Connection::Connection(Connection &&other) noexcept :
	socket_(std::move(other.socket_)), is_closed_(std::move(other.is_closed_)) { }

Connection	&Connection::operator=(Connection &&other) noexcept {
	socket_ = std::move(other.socket_);
	is_closed_ = std::move(other.is_closed_);
	return *this;
}

Connection::~Connection() { }

void		Connection::send(std::string &msg) {
	if (::send(socket_, msg.c_str(), msg.size(), 0) < 0) {
		is_closed_ = true;
	}
}

std::string	Connection::recive() {
	char		tmp[128];
	std::string	dst;
	int		ret_value = sizeof(tmp);

	while (ret_value == sizeof(tmp) && ((ret_value = recv(socket_, tmp, sizeof(tmp), 0)) > 0)) {
		dst += tmp;
	}
	if (ret_value == 0) {
		is_closed_ = true;
	}
	return dst;
}

bool		Connection::isClosed() const {
	return is_closed_;
}

int		Connection::getSocket__() const {
	return socket_;
}