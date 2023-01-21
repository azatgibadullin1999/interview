// #include "client.hpp"

# include "../socket/client_socket_manager.hpp"
# include <cstdlib>
# include <ctime>


namespace {

	std::string	generate_random_string() {
		std::string	dst;
		size_t		str_len;
		size_t		i;
		char		symbol;

		std::srand(std::time(NULL));
		str_len = 2 + (std::abs(std::rand()) % 6);
		dst.reserve(str_len + 1);
		while (i < str_len) {
			if (isalpha((symbol = std::abs((std::rand() % 70) + 60)))) {
				dst += symbol;
				++i;
			}
		}
		return dst;
	}

	std::string	read_from_standart_input() {
		char		tmp[128];
		std::string	msg;
		int		ret_value = sizeof(tmp);
		
		while (ret_value == sizeof(tmp) && ((ret_value = read(STDIN_FILENO, tmp, sizeof(tmp))) > 0)) {
			tmp[ret_value] = '\0';
			msg += tmp;
		}
		if (ret_value < 0) {
			throw std::exception();
		}
		return msg;
	}

	void		write_to_standart_output(const std::string &msg) {
		if (write(STDOUT_FILENO, msg.c_str(), msg.size()) < 0) {
			throw std::exception();
		}
	}
}

namespace	Client {

void	Start(const std::string &ip) {
	ClientSocketManager	client_socket(ip);
	Connection		con;
	std::string		msg;
	std::string		prefix = generate_random_string();

	std::cout << "started like client\nYour nickname for this session : " << prefix << std::endl;
	prefix = '[' + prefix + "] : ";

	while (true) {
		int ret = client_socket.poll();
		con = client_socket.getConnection();
		if (ret == 1) {
			msg = con.recive();
			if (con.isClosed()) {
				client_socket.endSession();
			}

			write_to_standart_output(msg);
		} else if (ret == 2) {
			msg = read_from_standart_input();
			if (msg.size() == 0) {
				continue;
			}

			con.send(prefix + msg);
			if (con.isClosed()) {
				client_socket.endSession();
			}
		}
	}
}

}