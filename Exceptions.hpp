#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

namespace wfs {
	class NoSuchFileException : public std::invalid_argument {
		public:
			NoSuchFileException() : invalid_argument("No such file") {};
	};
}

#endif
