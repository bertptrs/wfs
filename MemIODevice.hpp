#ifndef MEMIODEVICE_HPP
#define MEMIODEVICE_HPP

#include <vector>
#include <cstdint>
#include "IODevice.hpp"

namespace wfs {
	class MemIODevice : public IODevice {
		public:
			MemIODevice() = default;
			MemIODevice(const void* data, size_t len);

			virtual void read(size_t offset, size_t amount, void* buff);
			virtual void write(size_t offset, size_t amount, const void* buff);

		private:
			std::vector<uint8_t> backend;

			inline void ensureLength(size_t len);
	};
}

#endif

