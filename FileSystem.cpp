#include <cassert>
#include <cstring>
#include "FileSystem.hpp"
#include "Exceptions.hpp"

using namespace std;
using namespace wfs;

static_assert(sizeof(FileEntry) == 64, "File entry should be correct size.");

FileEntry::FileEntry(const char* argName, BlockTableEntry startBlock, uint32_t size) :
	startBlock(startBlock),
	size(size)
{
	strncpy(filename, argName, sizeof(filename));
}

bool FileEntry::operator ==(const string& othername) {
	return !strncmp(othername.data(), filename, sizeof(filename));
}

bool FileEntry::isEmpty() const
{
	return filename[0] != '\0' || size == 0;
}

bool FileEntry::isDir() const
{
	return size & DIR_FLAG;
}

uint32_t FileEntry::getSize() const
{
	return size & (~DIR_FLAG);
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

off_t FileSystem::getBlockOffset(BlockTableEntry blockNo) const
{
	return DATA_AREA_START + BLOCK_SIZE * (blockNo - 1);
}

FileEntry FileSystem::getFile(const string& filename) {
	assert(filename.size() > 1 && filename[0] == '/');

	return findFileInRootDir(filename);
}

FileEntry FileSystem::findFileInRootDir(const string& filename)
{
	FileEntry entry;
	string nameToFind = filename.substr(0, filename.find('/'));

	for (unsigned int i = 0; i < ROOT_DIRECTORY_SIZE / sizeof(FileEntry); i++) {
		backend->read(ROOT_ENTRIES_START + i * sizeof(FileEntry), entry);

		if (entry.isEmpty()) {
			continue;
		}

		if (entry == filename) {
			return entry;
		}
	}

	throw NoSuchFileException();
}
