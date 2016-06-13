#include <cxxtest/TestSuite.h>

#include <memory>
#include "../MemIODevice.hpp"
#include "../FileSystem.hpp"

using namespace wfs;
using namespace std;

class BlockTableTest : public CxxTest::TestSuite
{
	public:

		// Test for simple read/write in the blocktable.
		void testReadWrite(void)
		{
			BlockTable blockTable(make_shared<MemIODevice>(), 0);

			// Ensure there is nothing there.
			TS_ASSERT_EQUALS(0, blockTable.read(42));

			// Write a dummy value
			blockTable.write(42, 0xbeef);

			// Ensure its there.
			TS_ASSERT_EQUALS(0xbeef, blockTable.read(42));
		}

		// Test the allocate method.
		void testAllocate(void)
		{
			BlockTable blockTable(make_shared<MemIODevice>(), 0);
			verifyEmpty(blockTable);

			blockTable.write(2, 2); // Doesn't matter what we write, just not 0.
			blockTable.write(6, 2);

			BlockTableEntry start = blockTable.allocate(5);
			// Check that we got the first block.
			TS_ASSERT_EQUALS(1, start);

			// Check chain integrity.
			TS_ASSERT_EQUALS(3, blockTable.read(1));
			TS_ASSERT_EQUALS(4, blockTable.read(3));
			TS_ASSERT_EQUALS(5, blockTable.read(4));
			TS_ASSERT_EQUALS(7, blockTable.read(5));
			TS_ASSERT_EQUALS(BLOCK_EOF, blockTable.read(7));
		}

		// Test the allocate contiguous method.
		void testAllocateContiguous(void)
		{
			BlockTable blockTable(make_shared<MemIODevice>(), 0);

			verifyEmpty(blockTable);

			blockTable.write(2, 2); // Doesn't matter what we write, just not 0.
			blockTable.write(6, 2);

			BlockTableEntry start = blockTable.allocateContiguous(5);
			// Check that we got the first block.
			TS_ASSERT_EQUALS(7, start);

			// Check chain integrity.
			for (int i = 7; i < 7 + 5 - 1; i++) {
				TS_ASSERT_EQUALS(i + 1, blockTable.read(i));
			}
			TS_ASSERT_EQUALS(BLOCK_EOF, blockTable.read(11));
		}

		// Test the free chain method.
		void testFree(void)
		{
			BlockTable bt(make_shared<MemIODevice>(), 0);

			bt.write(42, 41);
			bt.write(41, 63);
			bt.write(63, 255);
			bt.write(255, 1);
			bt.write(1, BLOCK_EOF);

			bt.write(43, BLOCK_EOF);

			bt.free(42);

			// Ensure chain is freed.
			TS_ASSERT_EQUALS(BLOCK_FREE, bt.read(42));
			TS_ASSERT_EQUALS(BLOCK_FREE, bt.read(41));
			TS_ASSERT_EQUALS(BLOCK_FREE, bt.read(63));
			TS_ASSERT_EQUALS(BLOCK_FREE, bt.read(255));
			TS_ASSERT_EQUALS(BLOCK_FREE, bt.read(1));

			// Ensure no collateral.
			TS_ASSERT_EQUALS(BLOCK_EOF, bt.read(43));
		}

	private:
		void verifyEmpty(BlockTable& blockTable)
		{
			for (BlockTableEntry i = 1; i <= N_BLOCKS; i++) {
				TS_ASSERT_EQUALS(BLOCK_FREE, blockTable.read(i));
			}
		}
};
