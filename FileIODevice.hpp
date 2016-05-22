#ifndef FILEIODEVICE_HPP
#define FILEIODEVICE_HPP

#include <fstream>
#include <string>
#include "IODevice.hpp"

namespace wfs {
	class FileIODevice : public IODevice {
		public:
			FileIODevice(const std::string& filename);

			virtual void read(size_t offset, size_t amount, void* buff);
			virtual void write(size_t offset, size_t amount, const void* buff);

		private:
			std::fstream backend;
	};
}

#endif
