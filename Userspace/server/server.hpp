#ifndef USERSPACE_SERVER_SERVER_HPP_
# define USERSPACE_SERVER_SERVER_HPP_

# include <iostream>
# include <queue>
# include <exception>
# include <vector>
# include <thread>
# include <memory>

# include "../socket/server_socket_manager.hpp"


namespace Server {

	void	Start(const std::string &ip);

}


#endif // USERSPACE_SERVER_SERVER_HPP_