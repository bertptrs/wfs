#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <memory>
#include <cstdint>
#include <string>
#include "IODevice.hpp"

namespace wfs {
	const uint32_t MAGIC_NUMBERS[] = {
		0x00c0ffee,
		0x00000000,
		0xf00d1350,
		0x0000beef
	};
	typedef uint16_t BlockTableEntry;
	static constexpr size_t N_BLOCKS = 0x4000;
	static constexpr BlockTableEntry BLOCK_FREE = 0;
	static constexpr BlockTableEntry BLOCK_EOF = 0xfffe;

	struct __attribute__ ((__packed__)) FileEntry {
		char filename[58];
		BlockTableEntry startBlock;
		uint32_t size;

		FileEntry(const char* filename = "", BlockTableEntry startBlock = 0, uint32_t size = 0);

		constexpr static uint32_t DIR_FLAG = 1 << 31;

		bool isEmpty() const;
		bool isDir() const;
		uint32_t getSize() const;

		bool operator == (const std::string&);
	};

	class BlockTable {
		public:
			BlockTable(std::shared_ptr<IODevice> dev, off_t offset);

			BlockTableEntry read(BlockTableEntry pos);
			void write(BlockTableEntry pos, BlockTableEntry val);

			BlockTableEntry allocate(size_t number, BlockTableEntry prev = BLOCK_FREE);
			BlockTableEntry allocateContiguous(size_t number, BlockTableEntry prev = BLOCK_FREE);
			void free(BlockTableEntry start);

		private:
			const off_t offset;
			std::shared_ptr<IODevice> backend;

			inline off_t getBlockOffset(BlockTableEntry pos) const;
	};

	class FileSystem
	{
		public:


			FileSystem(std::shared_ptr<IODevice> dev);

			void init();

			FileEntry getFile(const std::string& filename);

		private:
			constexpr static size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBERS);
			constexpr static size_t ROOT_DIRECTORY_SIZE = 0x1000;
			constexpr static size_t BLOCK_TABLE_SIZE = N_BLOCKS * sizeof(BlockTableEntry);

			constexpr static off_t ROOT_ENTRIES_START = MAGIC_NUMBER_SIZE;
			constexpr static off_t BLOCK_TABLE_START = ROOT_ENTRIES_START + ROOT_DIRECTORY_SIZE;
			constexpr static off_t DATA_AREA_START = BLOCK_TABLE_START + BLOCK_TABLE_SIZE;
			constexpr static size_t BLOCK_SIZE = 0x0200;

			std::shared_ptr<IODevice> backend;
			BlockTable blockTable;

			inline off_t getBlockOffset(BlockTableEntry blockNo) const;

			FileEntry findFileInRootDir(const std::string& filename);
	};
}

#endif
