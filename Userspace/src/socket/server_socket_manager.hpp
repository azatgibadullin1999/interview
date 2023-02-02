#ifndef USERSPACE_SOCKETS_SOCKET_MANAGER_HPP_
# define USERSPACE_SOCKETS_SOCKET_MANAGER_HPP_

# include <exception>
# include <vector>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <unistd.h>
# include <poll.h>

// TMP LIBRARIS
# include <sys/ioctl.h>
# include <iostream>
# include <ctime>
# include "connection.hpp"
# include "socket_manager_base.hpp"
//

class ServerSocketManager final : private SocketManagerBase_ {
	public:
		ServerSocketManager(const std::string &ip);
		ServerSocketManager(const ServerSocketManager &other) = delete;
		ServerSocketManager&	operator=(const ServerSocketManager &other) = delete;
		ServerSocketManager(ServerSocketManager &&other) = delete;
		ServerSocketManager&	operator=(ServerSocketManager &&other) = delete;
		~ServerSocketManager();

		std::vector<Connection>	poll();
		void			deleteClient(Connection &con);
		void			sendAll(const std::string &msg, const Connection &from);
	private:
		void	addClient__();

		std::vector<pollfd>	pollfds_;
		sockaddr_in		addr_;
};

#endif // USERSPACE_SOCKETS_SOCKET_MANAGER_HPP_