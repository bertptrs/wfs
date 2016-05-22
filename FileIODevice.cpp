#include <cassert>
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
}

void FileIODevice::write(size_t offset, size_t amount, const void* buff)
{
	backend.seekp(offset);
	backend.write((const char*) buff, amount);
}
