#include "server/server.hpp"
#include "client/client.hpp"

int	main() {
	std::string	ip;

	std::cout << "Write ip : ";
	std::cin >> ip;
	try {
		Server::Start(ip);
	} catch (ft::BindError &e) {
		if (errno == EADDRINUSE) {
			Client::Start(ip);
		}
		else {
			throw;
		}
	}
	return 0;
}