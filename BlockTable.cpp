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

BlockTableEntry BlockTable::allocate(size_t num, BlockTableEntry prev)
{
	BlockTableEntry firstBlock = BLOCK_EOF;
	size_t free = 0;
	for (BlockTableEntry i = 1; i < N_BLOCKS && free < num; i++) {
		if (read(i) == BLOCK_FREE) {
			free++;

			if (firstBlock == BLOCK_EOF) {
				firstBlock = i;
			}
		}
	}

	if (free < num) {
		// Not enough free space, bail.
		return BLOCK_EOF;
	}

	// Mark found space as taken, and correct the chain.
	if (prev != BLOCK_FREE) {
		write(prev, firstBlock);
	}
	free = 1;
	BlockTableEntry prevBlock = firstBlock;
	for (BlockTableEntry i = firstBlock + 1; free < num; i++) {
		if (read(i) == BLOCK_FREE) {
			write(prevBlock, i);
			prevBlock = i;
			free++;
		}
	}

	write(prevBlock, BLOCK_EOF);

	return firstBlock;
}

BlockTableEntry BlockTable::allocateContiguous(size_t num, BlockTableEntry prev)
{
	BlockTableEntry firstBlock = BLOCK_EOF;
	size_t free = 0;
	for (BlockTableEntry i = 1; i < N_BLOCKS && free < num; i++) {
		if (read(i) == BLOCK_FREE) {
			free++;

			if (firstBlock == BLOCK_EOF) {
				firstBlock = i;
			}
		} else {
			firstBlock = BLOCK_EOF;
			free = 0;
		}
	}

	if (free < num) {
		// Not enough free space, bail.
		return BLOCK_EOF;
	}

	// Mark found space as taken, and correct the chain.
	if (prev != BLOCK_FREE) {
		write(prev, firstBlock);
	}
	for (BlockTableEntry i = firstBlock; i < firstBlock + num - 1; i++) {
		write(i, i + 1);
	}
	write(firstBlock + num - 1, BLOCK_EOF);

	return firstBlock;
}

void BlockTable::free(BlockTableEntry start)
{
	while (start != BLOCK_EOF) {
		assert(start >= 1 && start <= N_BLOCKS && "Corrupted blockchain.");

		BlockTableEntry next = read(start);

		write(start, BLOCK_FREE);

		start = next;
	}
}
