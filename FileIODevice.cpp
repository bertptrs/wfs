#include <cassert>
#include <cstring>
#include "FileIODevice.hpp"

using namespace std;
using namespace wfs;

FileIODevice::FileIODevice(const string& filename) :
	backend(filename, ios_base::in | ios_base::out | ios_base::binary)
{
	assert(backend.good() && "File should be okay.");
}

void FileIODevice::read(size_t offset, size_t amount, void* buff)
{
	backend.seekg(offset);
	backend.read((char*) buff, amount);

	if (backend.eof()) {
		// Did not read enough bytes. Set the remainder to zero.
		size_t bytesRead = backend.gcount();
		memset(((char*) buff) + bytesRead, 0, amount - bytesRead);
		backend.clear();
	}
}

void FileIODevice::write(size_t offset, size_t amount, const void* buff)
{
	backend.seekp(offset);
	backend.write((const char*) buff, amount);
}
