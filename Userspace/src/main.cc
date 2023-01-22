#include "server/server.hpp"
#include "client/client.hpp"

int	main() {
	std::string	ip;

	std::cout << "Write ip : ";
	std::cin >> ip;
	try {
		Client::Start(ip);
	} catch (ft::ConnectionError &e) {
		if (errno == ECONNREFUSED) {
			Server::Start(ip);
		}
	}
	Client::Start(ip);
	return 0;
}