#include "socket_manager_base.hpp"

SocketManagerBase_::SocketManagerBase_() {
	if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throw std::exception();
	}
}

SocketManagerBase_::~SocketManagerBase_() {
	close(socket_);
}