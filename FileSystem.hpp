#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <memory>
#include <cstdint>
#include "IODevice.hpp"

namespace wfs {
	const uint8_t MAGIC_NUMBERS[] = {
		0x00, 0xc0, 0xff, 0xee,
		0x00, 0x00, 0x00, 0x00,
		0xf0, 0x0d, 0x13, 0x50,
		0x00, 0x00, 0xbe, 0xef
	};
	class FileSystem
	{
		public:
			typedef uint16_t block_table_entry_t;
			struct __attribute__ ((__packed__)) file_entry_t {
				char filename[58];
				block_table_entry_t start_block;
				uint32_t size;
			};

			FileSystem(std::shared_ptr<IODevice> dev);

			void init();

		private:

			const static block_table_entry_t BLOCK_FREE = 0x0;
			const static block_table_entry_t BLOCK_EOF = 0xFFFE;

			const static size_t MAGIC_NUMBER_SIZE = sizeof(MAGIC_NUMBERS);
			const static size_t ROOT_DIRECTORY_SIZE = 0x1000;
			const static size_t N_BLOCKS = 0x4000;

			const static off_t BLOCK_TABLE_START = MAGIC_NUMBER_SIZE + ROOT_DIRECTORY_SIZE + N_BLOCKS * sizeof(file_entry_t);
			const static off_t ROOT_ENTRIES_START = MAGIC_NUMBER_SIZE;

			std::shared_ptr<IODevice> backend;

			off_t getBlockOffset(block_table_entry_t number);
			block_table_entry_t readBlockTable(block_table_entry_t number);
			void writeBlockTable(block_table_entry_t number, block_table_entry_t value);
	};
};

#endif
