#include <cassert>
#include "FileSystem.hpp"

using namespace std;
using namespace wfs;

static_assert(sizeof(FileSystem::file_entry_t) == 64, "File entry should be correct size.");

FileSystem::FileSystem(shared_ptr<IODevice> dev) :
	backend(dev)
{
}

void FileSystem::init()
{
	// Write magic numbers
	backend->write(0, sizeof(MAGIC_NUMBERS), MAGIC_NUMBERS);
	// Write free directory entries in the root
	file_entry_t emptyEntry;
	emptyEntry.start_block = 0;
	emptyEntry.filename[0] = '\0';
	emptyEntry.size = 0;
	for (size_t i = 0; i < ROOT_DIRECTORY_SIZE / sizeof(file_entry_t); i++) {
		backend->write(MAGIC_NUMBER_SIZE + i * sizeof(file_entry_t), emptyEntry);
	}

	// Write free block table
	for (size_t i = 1; i <= N_BLOCKS; i++) {
		writeBlockTable(i, BLOCK_FREE);
	}
}

off_t FileSystem::getBlockOffset(block_table_entry_t no)
{
	assert(no > 0 && "Block should be valid.");

	return BLOCK_TABLE_START + (no - 1) * sizeof(block_table_entry_t);
}


FileSystem::block_table_entry_t FileSystem::readBlockTable(FileSystem::block_table_entry_t no)
{
	block_table_entry_t value;

	backend->read(getBlockOffset(no), value);

	return value;
}

void FileSystem::writeBlockTable(FileSystem::block_table_entry_t no, FileSystem::block_table_entry_t value)
{
	backend->write(getBlockOffset(no), value);
}
