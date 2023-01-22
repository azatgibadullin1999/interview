#ifndef USERSPACE_SOCKET_CLIENT_SOCKET_MANAGER_HPP_
# define USERSPACE_SOCKET_CLIENT_SOCKET_MANAGER_HPP_


# include <exception>
# include <iostream>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <fcntl.h>

# include "connection.hpp"
# include "socket_manager_base.hpp"

# include <poll.h>


class ClientSocketManager final : private SocketManagerBase_ {
	public :
		ClientSocketManager(const std::string &ip);
		ClientSocketManager(const ClientSocketManager &other) = delete;
		ClientSocketManager&	operator=(const ClientSocketManager &other) = delete;
		ClientSocketManager(ClientSocketManager &&other) = delete;
		ClientSocketManager&	operator=(ClientSocketManager &&other) = delete;
		~ClientSocketManager();

		Connection	getConnection() const;
		int		poll();
		void		endSession();
		
	private :
		sockaddr_in	addr_;
		pollfd		pollfds_[2];

} ;


#endif // USERSPACE_SOCKET_CLIENT_SOCKET_MANAGER_HPP_