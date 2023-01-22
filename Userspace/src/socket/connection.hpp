#ifndef USERSPACE_SOCKET_CONNECTION_HPP_
# define USERSPACE_SOCKET_CONNECTION_HPP_

# include <string>
# include <sys/socket.h>
// TMP LIBS
# include <iostream>
//

class Connection final {
	public:
		Connection();
		Connection(int sock);
		Connection(const Connection &other);
		Connection	&operator=(const Connection &other);
		Connection(Connection &&other) noexcept;
		Connection	&operator=(Connection &&other) noexcept;
		~Connection();

		void		send(const std::string &msg);
		std::string	recive();
		bool		isClosed() const;
	private:
		int		getSocket__() const;
		int		socket_;
		bool		is_closed_;

		friend class ServerSocketManager;
		friend class ClientSocketManager;
};

#endif // USERSPACE_SOCKET_CONNECTION_HPP_