#include <cstring>
#include "MemIODevice.hpp"

using namespace std;
using namespace wfs;

MemIODevice::MemIODevice(const void* buf, size_t len) :
	backend(len)
{
	memcpy(backend.data(), buf, len);
}

void MemIODevice::read(size_t offset, size_t amount, void* buff)
{
	ensureLength(offset + amount);

	memcpy(buff, backend.data() + offset, amount);
}

void MemIODevice::write(size_t offset, size_t amount, const void* buff)
{
	ensureLength(offset + amount);

	memcpy(backend.data() + offset, buff, amount);
}

void MemIODevice::ensureLength(size_t len)
{
	if (backend.size() < len) {
		backend.resize(len);
	}
}
