#include <cassert>
#include "FileSystem.hpp"

using namespace std;
using namespace wfs;

BlockTable::BlockTable(shared_ptr<IODevice> dev, off_t offset) :
	offset(offset),
	backend(dev)
{
}

BlockTableEntry BlockTable::read(BlockTableEntry pos)
{
	BlockTableEntry value;

	backend->read(getBlockOffset(pos), value);

	return value;
}

void BlockTable::write(BlockTableEntry pos, BlockTableEntry val)
{
	backend->write(getBlockOffset(pos), val);
}

off_t BlockTable::getBlockOffset(BlockTableEntry pos) const
{
	assert(pos > 0 && pos <= N_BLOCKS);

	return offset + (pos - 1) * sizeof(BlockTableEntry);
}
