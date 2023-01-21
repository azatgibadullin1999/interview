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
	ClientSocketManager	cli(ip);
	Connection		con;
	std::string		msg;

	while (true) {
		int ret = cli.poll();
		con = cli.getConnection();
		if (ret == 1) {
			msg = con.recive();
			if (con.isClosed()) {
			}

			write_to_standart_output(msg);
		} else if (ret == 2) {
			msg = read_from_standart_input();

			con.send(msg);
			if (con.isClosed()) {
			}
		}
	}
}