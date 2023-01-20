#ifndef USERSPACE_SOCKET_SOCKET_MANAGER_BASE_HPP_
# define USERSPACE_SOCKET_SOCKET_MANAGER_BASE_HPP_


# include <exception>
# include <sys/socket.h>
# include <unistd.h>


class SocketManagerBase_ {
	public :
		SocketManagerBase_();
		SocketManagerBase_(const SocketManagerBase_ &other) = delete;
		SocketManagerBase_&	operator=(const SocketManagerBase_ &other) = delete;
		SocketManagerBase_(SocketManagerBase_ &&other) = delete;
		SocketManagerBase_&	operator=(SocketManagerBase_ &&other) = delete;
		~SocketManagerBase_();

	protected :
		int	socket_;
} ;


// class SocketError : public std::exception {
// 	public :
// 		SocketError() { }
// 		virtual ~SocketError() { }

// 		virtual const char	*what() const noexcept {
// 			return "Socket error";
// 		}
// } ;

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


#endif USERSPACE_SOCKET_SOCKET_MANAGER_BASE_HPP_