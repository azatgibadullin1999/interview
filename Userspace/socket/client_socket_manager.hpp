#ifndef USERSPACE_SOCKET_CLIENT_SOCKET_MANAGER_HPP_
# define USERSPACE_SOCKET_CLIENT_SOCKET_MANAGER_HPP_


# include <exception>
# include <iostream>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>

# include "connection.hpp"


class ClientSocketManager final {
	public :
		ClientSocketManager(const std::string &ip);
		ClientSocketManager(const ClientSocketManager &other) = delete;
		ClientSocketManager&	operator=(const ClientSocketManager &other) = delete;
		ClientSocketManager(ClientSocketManager &&other) = delete;
		ClientSocketManager&	operator=(ClientSocketManager &&other) = delete;
		~ClientSocketManager();

		Connection	getConnection() const;
		
	private :
		Connection	con_;
		sockaddr_in	addr_;

} ;


#endif // USERSPACE_SOCKET_CLIENT_SOCKET_MANAGER_HPP_