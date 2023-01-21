#include "client.hpp"


namespace {

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


void	Start(const std::string &ip) {
	ClientSocketManager	client_socket(ip);
	Connection		con;
	std::string		msg;
	std::string		prefix;

	std::cout << "started like client\nEnter a nickname for this session : ";
	std::cin >> prefix;

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

			con.send(prefix + msg);
			if (con.isClosed()) {
				client_socket.endSession();
			}
		}
	}
}