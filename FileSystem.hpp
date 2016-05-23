#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <memory>
#include <cstdint>
#include "IODevice.hpp"

namespace wfs {
	const uint32_t MAGIC_NUMBERS[] = {
		0x00c0ffee,
		0x00000000,
		0xf00d1350,
		0x0000beef
	};
	typedef uint16_t BlockTableEntry;

	class BlockTable {
		public:
			static constexpr size_t N_BLOCKS = 0x4000;
			static constexpr BlockTableEntry BLOCK_FREE = 0;
			static constexpr BlockTableEntry BLOCK_EOF = 0xfffe;

			BlockTable(std::shared_ptr<IODevice> dev, off_t offset);

			BlockTableEntry read(BlockTableEntry pos);
			void write(BlockTableEntry pos, BlockTableEntry val);

		private:
			const off_t offset;
			std::shared_ptr<IODevice> backend;

			inline off_t getBlockOffset(BlockTableEntry pos) const;
	};

	class FileSystem
	{
		public:
			struct __attribute__ ((__packed__)) FileEntry {
				char filename[58];
				BlockTableEntry startBlock;
				uint32_t size;

				FileEntry(const char* filename = "", BlockTableEntry startBlock = 0, uint32_t size = 0);
			};

			FileSystem(std::shared_ptr<IODevice> dev);

			void init();

		private:

			constexpr static BlockTableEntry BLOCK_FREE = 0x0;
			constexpr static BlockTableEntry BLOCK_EOF = 0xFFFE;

			constexpr static size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBERS);
			constexpr static size_t ROOT_DIRECTORY_SIZE = 0x1000;
			constexpr static size_t N_BLOCKS = 0x4000;
			constexpr static size_t BLOCK_TABLE_SIZE = N_BLOCKS * sizeof(BlockTableEntry);

			constexpr static off_t ROOT_ENTRIES_START = MAGIC_NUMBER_SIZE;
			constexpr static off_t BLOCK_TABLE_START = ROOT_ENTRIES_START + ROOT_DIRECTORY_SIZE;
			constexpr static off_t DATA_AREA_START = BLOCK_TABLE_START + BLOCK_TABLE_SIZE;

			std::shared_ptr<IODevice> backend;
			BlockTable blockTable;
	};
}

#endif
