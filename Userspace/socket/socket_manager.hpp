#ifndef USERSPACE_SOCKETS_SOCKET_MANAGER_HPP_
# define USERSPACE_SOCKETS_SOCKET_MANAGER_HPP_

# include <exception>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <unistd.h>
# include <poll.h>

// TMP LIBRARIS
# include <sys/ioctl.h>
# include <iostream>
# include "connection.hpp"
//

class SocketManager {
	public:
		SocketManager();
		SocketManager(const SocketManager &other) = delete;
		SocketManager&	operator=(const SocketManager &other) = delete;
		SocketManager(SocketManager &&other) = delete;
		SocketManager&	operator=(SocketManager &&other) = delete;
		~SocketManager();

		std::vector<Connection>	poll();
		void			deleteClient(Connection &con);
		void			sendAll(const std::string &msg, const Connection &from);
	private:
		void	addClient__();

		std::vector<pollfd>	pollfds_;
		sockaddr_in		addr_;
};

#endif // USERSPACE_SOCKETS_SOCKET_MANAGER_HPP_