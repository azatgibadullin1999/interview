#ifndef USERSPACE_SOCKET_SOCKET_MANAGER_BASE_HPP_
# define USERSPACE_SOCKET_SOCKET_MANAGER_BASE_HPP_


# include <exception>
# include <utility>
# include <sys/socket.h>
# include <unistd.h>


class SocketManagerBase_ {
	public :
		SocketManagerBase_();
		SocketManagerBase_(const SocketManagerBase_ &other) = delete;
		SocketManagerBase_&	operator=(const SocketManagerBase_ &other) = delete;
		SocketManagerBase_(SocketManagerBase_ &&other) = delete;
		SocketManagerBase_&	operator=(SocketManagerBase_ &&other) = delete;
		virtual ~SocketManagerBase_();

	protected :
		int	socket_;
} ;


namespace ft {

	struct SocketError : public std::exception {
		SocketError() { }
		virtual ~SocketError() { }
		virtual const char	*what() const noexcept {
			return "Socket error\n";
		}
	} ;

	struct ConnectionError : SocketError {
		ConnectionError() { }
		virtual ~ConnectionError() { }
		virtual const char	*what() const noexcept {
			return "connect() failed\n";
		}
	} ;

	struct BindError : SocketError {
		BindError() { }
		virtual ~BindError() { }
		virtual const char	*what() const noexcept {
			return "bind() failed\n";
		}
	} ;
}

// class SetsockportError : public SocketError {
// 	public :
// 		SetsockportError() { }
// 		virtual ~SetsockportError() { }

// 		virtual const char	*what() const noexcept {
// 			return "stsockport() Error";
// 		}
// } ;

// class IoctlError : public SocketError {
// 	public :
// 		IoctlError() { }
// 		virtual ~IoctlError() { }

// 		virtual const char	*what() const noexcept {
// 			return "ioctl() Error";
// 		}
// } ;

// class IoctlError : public SocketError {
// 	public :
// 		IoctlError() { }
// 		virtual ~IoctlError() { }

// 		virtual const char	*what() const noexcept {
// 			return "ioctl() Error";
// 		}
// } ;


#endif // USERSPACE_SOCKET_SOCKET_MANAGER_BASE_HPP_