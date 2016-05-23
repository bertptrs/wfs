#include <cassert>
#include <cstring>
#include "FileSystem.hpp"

using namespace std;
using namespace wfs;

static_assert(sizeof(FileSystem::FileEntry) == 64, "File entry should be correct size.");

FileSystem::FileEntry::FileEntry(const char* argName, BlockTableEntry startBlock, uint32_t size) :
	startBlock(startBlock),
	size(size)
{
	strncpy(filename, argName, sizeof(filename));
}


FileSystem::FileSystem(shared_ptr<IODevice> dev) :
	backend(dev),
	blockTable(dev, BLOCK_TABLE_START)
{
}

void FileSystem::init()
{
	// Write magic numbers
	backend->write(0, sizeof(MAGIC_NUMBERS), MAGIC_NUMBERS);
	// Write free directory entries in the root
	FileEntry emptyEntry("", BLOCK_FREE, 0);
	for (size_t i = 0; i < ROOT_DIRECTORY_SIZE / sizeof(FileEntry); i++) {
		backend->write(MAGIC_NUMBER_SIZE + i * sizeof(FileEntry), emptyEntry);
	}

	// Write free block table
	for (size_t i = 1; i <= N_BLOCKS; i++) {
		blockTable.write(i, BLOCK_FREE);
	}
}
